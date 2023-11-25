#include "parser/ast/stmt/ExprStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

ExprStmt::ExprStmt(std::shared_ptr<Expr> expr)
    : m_expr(expr)
{}

void ExprStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_expr->Accept(visitor);
    visitor->PostorderVisit(this);
}

const SourcePos& ExprStmt::StartPos() const
{
    return m_expr->StartPos();
}

} // namespace mylang
