#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

WhileStmt::WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body)
    : m_condition(condition)
    , m_body(body)
{}

void WhileStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_condition->Accept(visitor);
    m_body->Accept(visitor);
    visitor->PostorderVisit(this);
}

} // namespace mylang
