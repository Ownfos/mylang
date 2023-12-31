#include "parser/ast/expr/MemberAccessExpr.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <format>

namespace mylang
{

MemberAccessExpr::MemberAccessExpr(std::shared_ptr<Expr> expr, const Token& id)
    : m_expr(expr), m_id(id)
{}

void MemberAccessExpr::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& MemberAccessExpr::StartPos() const
{
    return m_expr->StartPos();
}

std::string MemberAccessExpr::ToString() const
{
    return std::format("{}.{}",
        m_expr->ToString(),
        m_id.lexeme
    );
}

Expr* MemberAccessExpr::Struct()
{
    return m_expr.get();
}

const Token& MemberAccessExpr::MemberName() const
{
    return m_id;
}

} // namespace mylang
