#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarInitExpr::VarInitExpr(std::shared_ptr<Expr> expr)
    : m_expr(expr)
{}

void VarInitExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_expr->Accept(visitor);
    visitor->PostorderVisit(this);
}

const Expr* VarInitExpr::Expression() const
{
    return m_expr.get();
}

} // namespace mylang
