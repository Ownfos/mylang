#include "parser/ast/visitor/TreePrinter.h"
#include "parser/ast/Program.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/globdecl/StructDecl.h"
#include <format>

namespace mylang
{

TreePrinter::TreePrinter(std::ostream& output_stream, int indent_spaces)
    : m_output_stream(output_stream)
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
    Indent();

    m_output_stream << std::format("Program: [name: {}, ", node->ModuleName().lexeme);
    m_output_stream << "imported modules: [";
    const auto& import_list = node->ImportList();
    for (int i = 0; i < import_list.size(); ++i)
    {
        if (i > 0)
        {
            m_output_stream << ", ";
        }

        const auto& [should_export, name] = import_list[i];
        m_output_stream << std::format("[should export: {}, name: {}]", should_export, name.lexeme);
    }
    m_output_stream << "]]\n";
}

void TreePrinter::Visit(FuncDecl* node)
{
    Indent();

    m_output_stream << std::format("FuncDecl: [name: {}, ", node->Name().lexeme);
    auto ret_type = node->ReturnType().value_or(Token{.lexeme = "void"});
    m_output_stream << std::format("return type: {}, ", ret_type.lexeme);
    m_output_stream << "parameters: [";
    const auto& parameters = node->Parameters();
    for (int i = 0; i < parameters.size(); ++i)
    {
        if (i > 0)
        {
            m_output_stream << ", ";
        }

        const auto& [name, usage, type] = parameters[i];
        m_output_stream << std::format("[name: {}, usage: {}, type: {}]",
            name.lexeme,
            usage.value_or(Token{.lexeme = "in"}).lexeme,
            type.lexeme
        );
    }
    m_output_stream << "]]\n";
}

void TreePrinter::Visit(StructDecl* node)
{
    Indent();
}

void TreePrinter::Indent()
{
    for (int i = 0; i < m_indent_level * m_indent_spaces; ++i)
    {
        m_output_stream << ' ';
    }
}

} // namespace mylang
