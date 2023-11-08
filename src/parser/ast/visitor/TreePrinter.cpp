#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"
#include "parser/ast/stmt/CompoundStmt.h"
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
        for (const auto& [name, usage, type] : node->Parameters())
        {
            Indent();
            m_output_stream << std::format("- parameter name: {}, usage: {}, type: {}\n",
                name.lexeme,
                usage ? usage->lexeme : "in",
                type->ToString()
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

void TreePrinter::Indent()
{
    for (int i = 0; i < m_indent_level * m_indent_spaces; ++i)
    {
        m_output_stream << ' ';
    }
}

} // namespace mylang
