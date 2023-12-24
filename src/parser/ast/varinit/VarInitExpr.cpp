#include "parser/ast/varinit/VarInitExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

VarInitExpr::VarInitExpr(std::shared_ptr<Expr> expr)
    : m_expr(expr)
{}

void VarInitExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& VarInitExpr::StartPos() const
{
    return m_expr->StartPos();
}

Expr* VarInitExpr::Expression()
{
    return m_expr.get();
}

} // namespace mylang
