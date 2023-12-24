#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

IfStmt::IfStmt(
    std::shared_ptr<Expr> condition,
    std::shared_ptr<Stmt> then_branch,
    std::shared_ptr<Stmt> else_branch
)
    : m_condition(condition)
    , m_then_branch(then_branch)
    , m_else_branch(else_branch)
{}

void IfStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& IfStmt::StartPos() const
{
    return m_condition->StartPos();
}

const Expr* IfStmt::Condition() const
{
    return m_condition.get();
}

} // namespace mylang
