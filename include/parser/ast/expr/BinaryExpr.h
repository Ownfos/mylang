#ifndef MYLANG_BINARY_EXPR_H
#define MYLANG_BINARY_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class BinaryExpr : public Expr
{
public:
    BinaryExpr(const Token& op, std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

    const Token& Operator() const;
    const Expr* LeftHandOperand() const;
    const Expr* RightHandOperand() const;

private:
    Token m_op;
    std::shared_ptr<Expr> m_lhs;
    std::shared_ptr<Expr> m_rhs;
};

} // namespace mylang


#endif // MYLANG_BINARY_EXPR_H
