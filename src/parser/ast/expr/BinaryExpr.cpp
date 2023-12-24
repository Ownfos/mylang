#include "parser/ast/expr/BinaryExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

BinaryExpr::BinaryExpr(const Token& op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs)
    : m_op(op), m_lhs(lhs), m_rhs(rhs)
{}

void BinaryExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& BinaryExpr::StartPos() const
{
    return m_lhs->StartPos();
}

std::string BinaryExpr::ToString() const
{
    return std::format("({} {} {})",
        m_lhs->ToString(),
        m_op.lexeme,
        m_rhs->ToString()
    );
}

const Token& BinaryExpr::Operator() const
{
    return m_op;
}

const Expr* BinaryExpr::LeftHandOperand() const
{
    return m_lhs.get();
}

const Expr* BinaryExpr::RightHandOperand() const
{
    return m_rhs.get();
}

} // namespace mylang
