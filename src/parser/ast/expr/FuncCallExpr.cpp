#include "parser/ast/expr/FuncCallExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>
#include <sstream>

namespace mylang
{

FuncCallExpr::FuncCallExpr(std::shared_ptr<Expr> expr, const std::vector<std::shared_ptr<Expr>>& arg_list)
    : m_expr(expr), m_arg_list(arg_list)
{}

void FuncCallExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& FuncCallExpr::StartPos() const
{
    return m_expr->StartPos();
}

std::string FuncCallExpr::ToString() const
{
    auto str_builder = std::ostringstream();
    str_builder << m_expr->ToString();
    str_builder << "(";

    // Parameter type list
    for (int i = 0; i < m_arg_list.size(); ++i)
    {
        // Add seperator for second to last paramters.
        if (i > 0)
        {
            str_builder << ", ";
        }

        str_builder << m_arg_list[i]->ToString();
    }
    str_builder << ")";

    return str_builder.str();
}

Expr* FuncCallExpr::Function()
{
    return m_expr.get();
}

const std::vector<std::shared_ptr<Expr>>& FuncCallExpr::ArgumentList() const
{
    return m_arg_list;
}

} // namespace mylang
