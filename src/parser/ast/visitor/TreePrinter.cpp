#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"
#include "parser/ast/stmt/CompoundStmt.h"
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

void TreePrinter::Visit(Program* node)
{
    // Node type
    Indent();
    m_output_stream << "[Program]\n";

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
            m_output_stream << std::format("- imported module: {}, should exported: {}\n",
                import_info.name.lexeme,
                import_info.should_export
            );
        }
    }
}

void TreePrinter::Visit(FuncDecl* node)
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
}

void TreePrinter::Visit(StructDecl* node)
{
    Indent();
}

void TreePrinter::Visit(CompoundStmt* node)
{
    Indent();
    m_output_stream << "[CompoundStmt]\n";
}

void TreePrinter::Visit(ArrayAccessExpr* node)
{
    Indent();
    m_output_stream << "[ArrayAccessExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(BinaryExpr* node)
{
    Indent();
    m_output_stream << "[BinaryExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(FuncCallExpr* node)
{
    Indent();
    m_output_stream << "[FuncCallExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(Identifier* node)
{
    Indent();
    m_output_stream << "[Identifier]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(Literal* node)
{
    Indent();
    m_output_stream << "[Literal]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(MemberAccessExpr* node)
{
    Indent();
    m_output_stream << "[MemberAccessExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(PostfixExpr* node)
{
    Indent();
    m_output_stream << "[PostfixExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Visit(PrefixExpr* node)
{
    Indent();
    m_output_stream << "[PrefixExpr]\n";
    if (m_verbose)
    {
        Indent();
        m_output_stream << "- " << node->ToString() << "\n";
    }
}

void TreePrinter::Indent()
{
    for (int i = 0; i < m_indent_level * m_indent_spaces; ++i)
    {
        m_output_stream << ' ';
    }
}

} // namespace mylang
