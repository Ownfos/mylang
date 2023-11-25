#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

ForStmt::ForStmt(
    std::shared_ptr<Stmt> initializer,
    std::shared_ptr<Expr> condition,
    std::shared_ptr<Expr> increment_expr,
    std::shared_ptr<Stmt> body
)
    : m_initializer(initializer)
    , m_condition(condition)
    , m_increment_expr(increment_expr)
    , m_body(body)
{}

void ForStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);

    // Note: m_initializer, m_condition, and m_increment_expr
    // are all optional, thus can be nullptr.
    if (m_initializer)
    {
        m_initializer->Accept(visitor);
    }

    if (m_condition)
    {
        m_condition->Accept(visitor);
    }

    if (m_increment_expr)
    {
        m_increment_expr->Accept(visitor);
    }

    m_body->Accept(visitor);
    
    visitor->PostorderVisit(this);
}

const SourcePos& ForStmt::StartPos() const
{
    // TODO: decide which value to return
    return {};
}

const Expr* ForStmt::Condition() const
{
    return m_condition.get();
}

} // namespace mylang
