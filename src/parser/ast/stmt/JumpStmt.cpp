#include "parser/ast/stmt/JumpStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

JumpStmt::JumpStmt(const Token& instruction, std::shared_ptr<Expr> expr)
    : m_jump_type(instruction)
    , m_expr(expr)
{}

void JumpStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    // Note: m_expr is not null only if this is a value-returning statement.
    if (m_expr)
    {
        m_expr->Accept(visitor);
    }
    visitor->PostorderVisit(this);
}

const SourcePos& JumpStmt::StartPos() const
{
    return m_jump_type.start_pos;
}

const Token& JumpStmt::JumpType() const
{
    return m_jump_type;
}

} // namespace mylang
