#ifndef MYLANG_POSTFIX_EXPR_H
#define MYLANG_POSTFIX_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class PostfixExpr : public Expr
{
public:
    PostfixExpr(const Token& op, std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

    const Token& Operator() const;
    Expr* Operand();
    
private:
    Token m_op;
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang


#endif // MYLANG_POSTFIX_EXPR_H
