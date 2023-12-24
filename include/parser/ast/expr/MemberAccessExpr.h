#ifndef MYLANG_MEMBER_ACCESS_EXPR_H
#define MYLANG_MEMBER_ACCESS_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class MemberAccessExpr : public Expr
{
public:
    MemberAccessExpr(std::shared_ptr<Expr> expr, const Token& id);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

    Expr* Struct();
    const Token& MemberName() const;

private:
    std::shared_ptr<Expr> m_expr;
    Token m_id;
};

} // namespace mylang


#endif // MYLANG_MEMBER_ACCESS_EXPR_H
