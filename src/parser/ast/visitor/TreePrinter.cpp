#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/Module.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"

#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/stmt/VarDeclStmt.h"
#include "parser/ast/stmt/ExprStmt.h"

#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/varinit/VarInitList.h"

#include "parser/ast/expr/ArrayAccessExpr.h"
#include "parser/ast/expr/BinaryExpr.h"
#include "parser/ast/expr/FuncCallExpr.h"
#include "parser/ast/expr/Identifier.h"
#include "parser/ast/expr/Literal.h"
#include "parser/ast/expr/MemberAccessExpr.h"
#include "parser/ast/expr/PostfixExpr.h"
#include "parser/ast/expr/PrefixExpr.h"
#include <format>

namespace mylang
{

TreePrinter::TreePrinter(std::ostream& output_stream, bool verbose, int indent_spaces)
    : m_output_stream(output_stream)
    , m_verbose(verbose)
    , m_indent_spaces(indent_spaces)
{}

void TreePrinter::IncreaseDepth()
{
    ++m_indent_level;
}

void TreePrinter::DecreaseDepth()
{
    --m_indent_level;
}

void TreePrinter::Visit(Module* node)
{
    PrintIndentedLine("[Module]");

    if (m_verbose)
    {
        PrintIndentedLine(std::format("- module: {}",
            node->ModuleName().lexeme
        ));

        for (const auto& import_info : node->ImportList())
        {
            PrintIndentedLine(std::format("- imported module: {}, export: {}",
                import_info.name.lexeme,
                import_info.should_export
            ));
        }
    }

    // Visit child node
    IncreaseDepth();
    for (const auto& decl : node->Declarations())
    {
        decl->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(Parameter* node)
{
    PrintIndentedLine("[Parameter]");

    if (m_verbose)
    {
        PrintIndentedLine(std::format("- name: {}",
            node->Name().lexeme
        ));

        PrintIndentedLine(std::format("- type: {}",
            node->DeclParamType().ToString()
        ));
    }
}

void TreePrinter::Visit(FuncDecl* node)
{
    PrintIndentedLine("[FuncDecl]");

    if (m_verbose)
    {
        PrintIndentedLine(std::format("- name: {}",
            node->Name().lexeme
        ));

        // Is this symbol visible in other modules?
        PrintIndentedLine(std::format("- export: {}",
            node->ShouldExport()
        ));

        auto ret_type = node->ReturnType();
        PrintIndentedLine(std::format("- return type: {}",
            ret_type.ToString()
        ));
    }
    
    // Visit child node
    IncreaseDepth();
    for (const auto& param : node->Parameters())
    {
        param->Accept(this);
    }
    node->Body()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(StructDecl* node)
{
    PrintIndentedLine("[StructDecl]");

    if (m_verbose)
    {
        PrintIndentedLine(std::format("- name: {}",
            node->Name().lexeme
        ));

        // Is this symbol visible in other modules?
        PrintIndentedLine(std::format("- export: {}",
            node->ShouldExport()
        ));

        for (const auto& member : node->Members())
        {            
            PrintIndentedLine(std::format("- member name: {}, type: {}",
                member.name.lexeme,
                member.type.ToString()
            ));
        }
    }
}

void TreePrinter::Visit(CompoundStmt* node)
{
    
    PrintIndentedLine("[CompoundStmt]");

    // Visit child node
    IncreaseDepth();
    for (const auto& stmt : node->Statements())
    {
        stmt->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(IfStmt* node)
{
    PrintIndentedLine("[IfStmt]");

    // Visit child node
    // NOTE: else branch is optional, so it might be nullptr.
    IncreaseDepth();
    node->Condition()->Accept(this);
    node->ThenBranch()->Accept(this);
    if (auto else_branch = node->ElseBranch())
    {
        else_branch->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(ForStmt* node)
{
    PrintIndentedLine("[ForStmt]");

    // Visit child node
    // NOTE: everything except the statememt body is optional!
    IncreaseDepth();
    if (auto initializer = node->Initializer())
    {
        initializer->Accept(this);
    }
    if (auto condition = node->Condition())
    {
        condition->Accept(this);
    }
    if (auto increment = node->IncrementExpr())
    {
        increment->Accept(this);
    }
    node->Body()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(WhileStmt* node)
{
    PrintIndentedLine("[WhileStmt]");

    // Visit child node
    IncreaseDepth();
    node->Condition()->Accept(this);
    node->Body()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(JumpStmt* node)
{
    PrintIndentedLine("[JumpStmt]");

    if (m_verbose)
    {
        PrintIndentedLine(std::format("- jump type: {}",
            node->JumpType().lexeme
        ));
    }

    // Visit child node
    // NOTE: return value doesn't exist for "break" and "continue"
    IncreaseDepth();
    if (auto ret = node->ReturnValueExpr())
    {
        ret->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(VarDeclStmt* node)
{
    PrintIndentedLine("[VarDeclStmt]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- name: {}",
            node->Name().lexeme
        ));

        PrintIndentedLine(std::format("- type: {}",
            node->DeclType().ToString()
        ));
    }

    // Visit child node
    // NOTE: initializer part is optional (e.g. "abc: i32;")
    IncreaseDepth();
    if (auto initializer = node->Initializer())
    {
        initializer->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(ExprStmt* node)
{
    PrintIndentedLine("[ExprStmt]");

    // Visit child node
    IncreaseDepth();
    node->Expression()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(VarInitExpr* node)
{   
    PrintIndentedLine("[VarInitExpr]");

    // Visit child node
    IncreaseDepth();
    node->Expression()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(VarInitList* node)
{
    PrintIndentedLine("[VarInitList]");

    // Visit child node
    IncreaseDepth();
    for (auto list_elem : node->InitializerList())
    {
        list_elem->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(ArrayAccessExpr* node)
{
    PrintIndentedLine("[ArrayAccessExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->Operand()->Accept(this);
    node->Index()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(BinaryExpr* node)
{
    PrintIndentedLine("[BinaryExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->LeftHandOperand()->Accept(this);
    node->RightHandOperand()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(FuncCallExpr* node)
{
    PrintIndentedLine("[FuncCallExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->Function()->Accept(this);
    for (const auto& arg : node->ArgumentList())
    {
        arg->Accept(this);
    }
    DecreaseDepth();
}

void TreePrinter::Visit(Identifier* node)
{
    PrintIndentedLine("[Identifier]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }
}

void TreePrinter::Visit(Literal* node)
{
    PrintIndentedLine("[Literal]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }
}

void TreePrinter::Visit(MemberAccessExpr* node)
{
    PrintIndentedLine("[MemberAccessExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->Struct()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(PostfixExpr* node)
{
    PrintIndentedLine("[PostfixExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->Operand()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::Visit(PrefixExpr* node)
{
    PrintIndentedLine("[PrefixExpr]");
    if (m_verbose)
    {
        PrintIndentedLine(std::format("- {}", node->ToString()));
    }

    // Visit child node
    IncreaseDepth();
    node->Operand()->Accept(this);
    DecreaseDepth();
}

void TreePrinter::PrintIndentedLine(std::string_view msg)
{
    const auto indentation = std::string(m_indent_level * m_indent_spaces, ' ');
    m_output_stream << indentation << msg << '\n';
}

} // namespace mylang
