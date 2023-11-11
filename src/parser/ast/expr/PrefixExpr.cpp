#include "parser/ast/expr/PrefixExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

PrefixExpr::PrefixExpr(const Token& op, std::shared_ptr<Expr> expr)
    : m_op(op), m_expr(expr)
{}

void PrefixExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    visitor->IncreaseDepth();
    m_expr->Accept(visitor);
    visitor->DecreaseDepth();
}

std::string PrefixExpr::ToString() const
{
    return std::format("({} {})",
        m_op.lexeme,
        m_expr->ToString()
    );
}

} // namespace mylang
