#include "codegen/CodeGenerator.h"
#include "parser/ast/Module.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"
#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/varinit/VarInitList.h"

namespace mylang
{

CodeGenerator::CodeGenerator(
    ProgramEnvironment& environment,
    const std::filesystem::path& output_dir,
    std::unique_ptr<IOutputFileFactory>&& file_factory
)
    : m_environment(environment)
    , m_output_dir(output_dir)
    , m_file_factory(std::move(file_factory))
{}

CodeGenerator::~CodeGenerator()
{
    CloseAllFiles();
}

IOutputFile* CodeGenerator::GetFile(const std::string& file_name)
{
    auto it = m_open_files.find(file_name);

    // If we have an open file, return it.
    if (it != m_open_files.end())
    {
        return it->second.get();
    }
    // Create new one if it doesn't exist.
    else
    {
        auto new_file = m_file_factory->CreateOutputFile();
        new_file->Open(m_output_dir / file_name);
        m_open_files.emplace(file_name, new_file);

        return new_file.get();
    }
}

void CodeGenerator::CloseAllFiles()
{
    for (auto& [name, file] : m_open_files)
    {
        file->Close();
    }
}

bool CodeGenerator::IsModuleNodeVisited(const std::string& module_name) const
{
    return m_visited_modules.count(module_name) > 0;
}

void CodeGenerator::MarkModuleAsVisited(const std::string& module_name)
{
    m_visited_modules.insert(module_name);
}

std::string HeaderGuardMacro(const std::string& module_name)
{
    return std::format("MODULE_{}_H", module_name);
}

std::string HeaderFileName(const std::string& module_name)
{
    return std::format("{}.h", module_name);
}

std::string SourceFileName(const std::string& module_name)
{
    return std::format("{}.cpp", module_name);
}

std::string IncludeHeaderMacro(const std::string& module_name)
{
    return std::format("#include \"{}\"\n", HeaderFileName(module_name));
}

std::string FunctionSignature(FuncDecl* func)
{
    return "";
}

void CodeGenerator::InitializeHeaderFile(const std::string& module_name)
{
    auto header_file = GetFile(HeaderFileName(module_name));

    // Open header guard.
    auto header_guard = HeaderGuardMacro(module_name);
    header_file->Print(std::format("#ifndef {}\n", header_guard));
    header_file->Print(std::format("#define {}\n", header_guard));
    header_file->Print("#include <functional>\n");

    // Handle global import directives and symbols.
    const auto& module_info = m_environment.GetModuleInfo(module_name);
    EmitImportDirectives(header_file, module_info.import_list, true);
    EmitForwardDecl(header_file, module_info.local_symbol_table.GlobalSymbols());

    // Close header guard.
    header_file->Print(std::format("#endif // {}\n", header_guard));
}

void CodeGenerator::InitializeSourceFile(const std::string& module_name)
{
    auto source_file = GetFile(SourceFileName(module_name));

    // Import this module's header file
    source_file->Print(IncludeHeaderMacro(module_name));

    // Handle local import directives and symbols.
    const auto& module_info = m_environment.GetModuleInfo(module_name);
    EmitImportDirectives(source_file, module_info.import_list, false);
    EmitForwardDecl(source_file, module_info.local_symbol_table.LocalSymbols());
}

void CodeGenerator::EmitImportDirectives(IOutputFile* output_file, const std::set<ModuleImportInfo>& import_directives, bool export_qualifier)
{
    for (const auto& import_directive : import_directives)
    {
        if (import_directive.should_export == export_qualifier)
        {
            output_file->Print(IncludeHeaderMacro(import_directive.name.lexeme));
        }
    }
}

void CodeGenerator::EmitForwardDecl(IOutputFile* output_file, const std::vector<Symbol>& symbols)
{
    // Set output target.
    m_current_output_file = output_file;

    // Prevent recursive tree traversal.
    // We do not want symbol definitions such as function body.
    m_is_forward_decl_step = true;
    
    for (const auto& symbol : symbols)
    {
        symbol.declaration->Accept(this);
    }

    // Enable recursive tree traversal.
    m_is_forward_decl_step = false;
}

void CodeGenerator::Visit(Module* node)
{
    const auto& module_name = node->ModuleName().lexeme;

    // Handle per-module actions such as header file generation.
    if (!IsModuleNodeVisited(module_name))
    {
        MarkModuleAsVisited(module_name);

        InitializeHeaderFile(module_name);
        InitializeSourceFile(module_name);
    }

    // From now on, all implementation codes will be emitted to this file.
    m_current_output_file = GetFile(SourceFileName(module_name));

    for (const auto& decl : node->Declarations())
    {
        decl->Accept(this);
    }
}

void CodeGenerator::Visit(FuncDecl* node)
{
    m_current_output_file->Print(std::format("{} {}(",
        node->ReturnType().ToCppString(),
        node->Name().lexeme
    ));
    const auto& params = node->Parameters();
    for (int i = 0; i < params.size(); ++i)
    {
        // Add seperator for second to last paramters.
        if (i > 0)
        {
            m_current_output_file->Print(", ");
        }

        m_current_output_file->Print(std::format("{} {}", 
            params[i]->DeclParamType().ToCppString(),
            params[i]->Name().lexeme
        ));
    }
    m_current_output_file->Print(")");

    if (m_is_forward_decl_step)
    {
        m_current_output_file->Print(";\n");
    }
    else
    {
        m_current_output_file->Print(" ");
        node->Body()->Accept(this);
    }
}

void CodeGenerator::Visit(StructDecl* node)
{
    // In C++, we cannot use a struct type before providing the definition.
    // This means that forward declaration doesn't work as in functions!
    // We should instead put struct definitions on the top.
    //
    // Note: this doesn't solve dependency issue among struct members.
    //       ex) struct A using struct B as member (which comes afterward)
    //           will generate a compiler error...
    if (m_is_forward_decl_step)
    {
        m_current_output_file->Print(std::format("struct {}", node->Name().lexeme));
        m_current_output_file->Print(" {\n");
        m_current_output_file->IncreaseDepth();
        for (const auto& member : node->Members())
        {
            m_current_output_file->PrintIndented(std::format("{} {};\n",
               member.type.ToCppString(),
               member.name.lexeme
            ));
        }
        m_current_output_file->DecreaseDepth();
        m_current_output_file->Print("};\n");
    }
}

void CodeGenerator::Visit(CompoundStmt* node)
{
    m_current_output_file->PrintIndented("{\n");
    m_current_output_file->IncreaseDepth();
    for (const auto& stmt : node->Statements())
    {
        stmt->Accept(this);
    }
    m_current_output_file->DecreaseDepth();
    m_current_output_file->PrintIndented("}\n");
}

void CodeGenerator::Visit(VarDeclStmt* node)
{
    const auto& var_type = node->DeclType();
    m_current_output_file->PrintIndented(std::format("{} {}",
        var_type.ToCppString(),
        node->Name().lexeme
    ));

    // Handle optional initializer part.
    if (auto initializer = node->Initializer())
    {
        m_current_output_file->Print(" = ");
        if (var_type.IsArray())
        {
            m_current_output_file->Print("{");
        }

        initializer->Accept(this);
        
        if (var_type.IsArray())
        {
            m_current_output_file->Print("}");
        }
        m_current_output_file->Print(";\n");
    }
    else
    {
        m_current_output_file->Print(";\n");
    }
}

void CodeGenerator::Visit(VarInitExpr* node)
{
    m_current_output_file->Print(node->Expression()->ToString());
}

void CodeGenerator::Visit(VarInitList* node)
{
    m_current_output_file->Print("{");
    const auto& list_elem = node->InitializerList();
    for (int i = 0; i < list_elem.size(); ++i)
    {
        // Separator between elements.
        if (i > 0)
        {
            m_current_output_file->Print(", ");
        }

        list_elem[i]->Accept(this);
    }
    m_current_output_file->Print("}");
}

void CodeGenerator::Visit(ExprStmt* node)
{
    m_current_output_file->PrintIndented(std::format("{};\n", node->Expression()->ToString()));
}

void CodeGenerator::Visit(IfStmt* node)
{
    m_current_output_file->PrintIndented(std::format("if ({}) ", node->Condition()->ToString()));
    m_current_output_file->DisableNextIndentation(); // We want to place then-branch at the same line.
    node->ThenBranch()->Accept(this);

    if (auto else_branch = node->ElseBranch())
    {
        m_current_output_file->PrintIndented("else ");
        m_current_output_file->DisableNextIndentation(); // We want to place else-branch at the same line.
        else_branch->Accept(this);
    }
}

void CodeGenerator::Visit(ForStmt* node)
{
    // Create a nested scope to prevent the variable declared in the initializer
    // from being visible on the scope outside the loop body.
    // ex) "for (init; cond; inc) body" gets translated to the following C++ code:
    // {
    //     init;
    //     while (true) {
    //         if (cond == false) break;
    //         body;
    //         inc;
    //     }
    // }
    m_current_output_file->PrintIndented("{\n");
    m_current_output_file->IncreaseDepth();

    if (auto initializer = node->Initializer())
    {
        initializer->Accept(this);
    }

    m_current_output_file->PrintIndented("while (true) {\n");
    m_current_output_file->IncreaseDepth();

    if (auto condition = node->Condition())
    {
        m_current_output_file->PrintIndented(std::format("if ({} == false) break;\n", condition->ToString()));
    }

    node->Body()->Accept(this);
    
    if (auto increment = node->IncrementExpr())
    {
        m_current_output_file->PrintIndented(increment->ToString());
        m_current_output_file->Print(";\n");
    }
    
    m_current_output_file->DecreaseDepth();
    m_current_output_file->PrintIndented("}\n"); // End of while (true){...}

    m_current_output_file->DecreaseDepth();
    m_current_output_file->PrintIndented("}\n"); // End of the outermost block "{ while (true){ ... } }"
}

void CodeGenerator::Visit(WhileStmt* node)
{
    m_current_output_file->PrintIndented(std::format("while ({}) ", node->Condition()->ToString()));
    m_current_output_file->DisableNextIndentation(); // We want to place the loop body at the same line.
    node->Body()->Accept(this);
}

void CodeGenerator::Visit(JumpStmt* node)
{
    m_current_output_file->PrintIndented(node->JumpType().lexeme);
    if (auto return_expr = node->ReturnValueExpr())
    {
        m_current_output_file->Print(" ");
        m_current_output_file->Print(return_expr->ToString());
    }
    m_current_output_file->Print(";\n");
}

} // namespace mylang
