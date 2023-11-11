#ifndef MYLANG_ARRAY_ACCESS_EXPR_H
#define MYLANG_ARRAY_ACCESS_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class ArrayAccessExpr : public Expr
{
public:
    ArrayAccessExpr(std::shared_ptr<Expr> expr, std::shared_ptr<Expr> index);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual std::string ToString() const override;

private:
    std::shared_ptr<Expr> m_expr;
    std::shared_ptr<Expr> m_index;
};

} // namespace mylang


#endif // MYLANG_ARRAY_ACCESS_EXPR_H
