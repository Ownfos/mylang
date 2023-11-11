#ifndef MYLANG_FOR_STMT_H
#define MYLANG_FOR_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class ForStmt : public Stmt
{
public:
    // 'initializer' should be either VarDeclStmt or ExprStmt.
    // 'condition' and 'increment_expr' could be nullptr.
    // 'body' is expected to be a compound statement.
    ForStmt(
        std::shared_ptr<Stmt> initializer,
        std::shared_ptr<Expr> condition,
        std::shared_ptr<Expr> increment_expr,
        std::shared_ptr<Stmt> body
    );

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

private:
    std::shared_ptr<Stmt> m_initializer;
    std::shared_ptr<Expr> m_condition;
    std::shared_ptr<Expr> m_increment_expr;
    std::shared_ptr<Stmt> m_body;
};

} // namespace mylang


#endif // MYLANG_FOR_STMT_H
