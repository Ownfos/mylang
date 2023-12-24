#include "parser/ast/expr/PrefixExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

PrefixExpr::PrefixExpr(const Token& op, std::shared_ptr<Expr> expr)
    : m_op(op), m_expr(expr)
{}

void PrefixExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& PrefixExpr::StartPos() const
{
    return m_op.start_pos;
}

std::string PrefixExpr::ToString() const
{
    return std::format("({}{})",
        m_op.lexeme,
        m_expr->ToString()
    );
}

const Token& PrefixExpr::Operator() const
{
    return m_op;
}

Expr* PrefixExpr::Operand()
{
    return m_expr.get();
}

} // namespace mylang
