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
    visitor->Visit(this);
    
    visitor->IncreaseDepth();
    // Note: m_expr is not null only if this is a value-returning statement.
    if (m_expr)
    {
        m_expr->Accept(visitor);
    }
    visitor->DecreaseDepth();
}

const Token& JumpStmt::JumpType() const
{
    return m_jump_type;
}

} // namespace mylang
