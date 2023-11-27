#ifndef MYLANG_PREFIX_EXPR_H
#define MYLANG_PREFIX_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class PrefixExpr : public Expr
{
public:
    PrefixExpr(const Token& op, std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

    const Token& Operator() const;
    const Expr* Operand() const;

private:
    Token m_op;
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang


#endif // MYLANG_PREFIX_EXPR_H
