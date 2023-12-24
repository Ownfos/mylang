#include "parser/ast/stmt/ExprStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

ExprStmt::ExprStmt(std::shared_ptr<Expr> expr)
    : m_expr(expr)
{}

void ExprStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& ExprStmt::StartPos() const
{
    return m_expr->StartPos();
}

Expr* ExprStmt::Expression()
{
    return m_expr.get();
}

} // namespace mylang
