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
    visitor->Visit(this);
}

const SourcePos& WhileStmt::StartPos() const
{
    return m_condition->StartPos();
}

Expr* WhileStmt::Condition()
{
    return m_condition.get();
}

Stmt* WhileStmt::Body()
{
    return m_body.get();
}

} // namespace mylang
