#include "parser/ast/expr/PostfixExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

PostfixExpr::PostfixExpr(const Token& op, std::shared_ptr<Expr> expr)
    : m_op(op), m_expr(expr)
{}

void PostfixExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    m_expr->Accept(visitor);
    visitor->PostorderVisit(this);
}

const SourcePos& PostfixExpr::StartPos() const
{
    return m_expr->StartPos();
}

std::string PostfixExpr::ToString() const
{
    return std::format("({}{})",
        m_expr->ToString(),
        m_op.lexeme
    );
}

} // namespace mylang
