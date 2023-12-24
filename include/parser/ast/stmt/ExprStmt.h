#ifndef MYLANG_EXPR_STMT_H
#define MYLANG_EXPR_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class ExprStmt : public Stmt
{
public:
    ExprStmt(std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    Expr* Expression();

private:
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang


#endif // MYLANG_EXPR_STMT_H
