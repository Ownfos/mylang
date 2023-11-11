#include "parser/ast/expr/ArrayAccessExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

ArrayAccessExpr::ArrayAccessExpr(std::shared_ptr<Expr> expr, std::shared_ptr<Expr> index)
    : m_expr(expr), m_index(index)
{}

void ArrayAccessExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    visitor->IncreaseDepth();
    m_expr->Accept(visitor);
    m_index->Accept(visitor);
    visitor->DecreaseDepth();
}

std::string ArrayAccessExpr::ToString() const
{
    return std::format("{}[{}]",
        m_expr->ToString(),
        m_index->ToString()
    );
}

} // namespace mylang
