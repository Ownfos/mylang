#ifndef MYLANG_VAR_INIT_EXPR_H
#define MYLANG_VAR_INIT_EXPR_H

#include "parser/ast/varinit/VarInit.h"
#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class VarInitExpr : public VarInit
{
public:
    VarInitExpr(std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    const Expr* Expression() const;

private:
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang

#endif // MYLANG_VAR_INIT_EXPR_H
