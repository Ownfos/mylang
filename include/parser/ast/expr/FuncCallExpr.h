#ifndef MYLANG_FUNC_CALL_EXPR_H
#define MYLANG_FUNC_CALL_EXPR_H

#include "parser/ast/expr/Expr.h"
#include <memory>
#include <vector>

namespace mylang
{

class FuncCallExpr : public Expr
{
public:
    FuncCallExpr(std::shared_ptr<Expr> expr, const std::vector<std::shared_ptr<Expr>>& arg_list);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

    const Expr* Function() const;
    const std::vector<std::shared_ptr<Expr>>& ArgumentList() const;

private:
    std::shared_ptr<Expr> m_expr;
    std::vector<std::shared_ptr<Expr>> m_arg_list;
};

} // namespace mylang


#endif // MYLANG_FUNC_CALL_EXPR_H
