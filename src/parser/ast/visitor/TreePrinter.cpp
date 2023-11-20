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

void TreePrinter::PreorderVisit(Module* node)
{
    // Node type
    Indent();
    m_output_stream << "[Module]\n";

    if (m_verbose)
    {
        // Module declaration
        Indent();
        m_output_stream << std::format("- module: {}\n",
            node->ModuleName().lexeme
        );

        // Imported modules
        for (const auto& import_info : node->ImportList())
        {
            Indent();
            m_output_stream << std::format("- imported module: {}, export: {}\n",
                import_info.name.lexeme,
                import_info.should_export
            );
        }
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(FuncDecl* node)
{
    // Node type
    Indent();
    m_output_stream << "[FuncDecl]\n";

    if (m_verbose)
    {
        // Function name
        Indent();
        m_output_stream << std::format("- name: {}\n",
            node->Name().lexeme
        );

        // Is this symbol visible in other modules?
        Indent();
        m_output_stream << std::format("- export: {}\n",
            node->ShouldExport()
        );

        // Return type
        Indent();
        auto ret_type = node->ReturnType();
        m_output_stream << std::format("- return type: {}\n",
            ret_type ? ret_type->ToString() : "void"
        );

        // Parameters
        for (const auto& [name, type] : node->Parameters())
        {
            Indent();
            m_output_stream << std::format("- parameter name: {}, type: {}\n",
                name.lexeme,
                type.ToString()
            );
        }
    }
    
    IncreaseDepth();
}

void TreePrinter::PreorderVisit(StructDecl* node)
{
    Indent();
    m_output_stream << "[StructDecl]\n";

    if (m_verbose)
    {
        // Struct name
        Indent();
        m_output_stream << std::format("- name: {}\n",
            node->Name().lexeme
        );

        // Is this symbol visible in other modules?
        Indent();
        m_output_stream << std::format("- export: {}\n",
            node->ShouldExport()
        );

        // Member variables
        for (const auto& member : node->Members())
        {
            Indent();
            m_output_stream << std::format("- member name: {}, type: {}\n",
                member.name.lexeme,
                member.type.ToString()
            );
        }
    }
}

void TreePrinter::PreorderVisit(CompoundStmt* node)
{
    Indent();
    m_output_stream << "[CompoundStmt]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(IfStmt* node)
{
    Indent();
    m_output_stream << "[IfStmt]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(ForStmt* node)
{
    Indent();
    m_output_stream << "[ForStmt]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(WhileStmt* node)
{
    Indent();
    m_output_stream << "[WhileStmt]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(JumpStmt* node)
{
    Indent();
    m_output_stream << "[JumpStmt]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << std::format("- jump type: {}\n",
            node->JumpType().lexeme
        );
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(VarDeclStmt* node)
{
    Indent();
    m_output_stream << "[VarDeclStmt]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << std::format("- name: {}\n",
            node->Name().lexeme
        );

        Indent();
        m_output_stream << std::format("- type: {}\n",
            node->DeclType().ToString()
        );
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(ExprStmt* node)
{
    Indent();
    m_output_stream << "[ExprStmt]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(VarInitExpr* node)
{
    Indent();
    m_output_stream << "[VarInitExpr]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(VarInitList* node)
{
    Indent();
    m_output_stream << "[VarInitList]\n";

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(ArrayAccessExpr* node)
{
    Indent();
    m_output_stream << "[ArrayAccessExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::PreorderVisit(BinaryExpr* node)
{
    Indent();
    m_output_stream << "[BinaryExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(FuncCallExpr* node)
{
    Indent();
    m_output_stream << "[FuncCallExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(Identifier* node)
{
    Indent();
    m_output_stream << "[Identifier]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::PreorderVisit(Literal* node)
{
    Indent();
    m_output_stream << "[Literal]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::PreorderVisit(MemberAccessExpr* node)
{
    Indent();
    m_output_stream << "[MemberAccessExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(PostfixExpr* node)
{
    Indent();
    m_output_stream << "[PostfixExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }

    IncreaseDepth();
}

void TreePrinter::PreorderVisit(PrefixExpr* node)
{
    Indent();
    m_output_stream << "[PrefixExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }

    IncreaseDepth();
}

void TreePrinter::PostorderVisit(Module* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(FuncDecl* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(StructDecl* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(CompoundStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(IfStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(ForStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(WhileStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(JumpStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(VarDeclStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(ExprStmt* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(VarInitExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(VarInitList* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(ArrayAccessExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(BinaryExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(FuncCallExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(MemberAccessExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(PostfixExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::PostorderVisit(PrefixExpr* node)
{
    DecreaseDepth();
}

void TreePrinter::Indent()
{
    for (int i = 0; i < m_indent_level * m_indent_spaces; ++i)
    {
        m_output_stream << ' ';
    }
}

} // namespace mylang
