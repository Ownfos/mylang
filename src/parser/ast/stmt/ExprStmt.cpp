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
    
    visitor->IncreaseDepth();
    m_expr->Accept(visitor);
    visitor->DecreaseDepth();
}

} // namespace mylang
