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
    visitor->Visit(this);
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
