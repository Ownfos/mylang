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
    
    visitor->IncreaseDepth();
    m_expr->Accept(visitor);
    visitor->DecreaseDepth();
}

} // namespace mylang
