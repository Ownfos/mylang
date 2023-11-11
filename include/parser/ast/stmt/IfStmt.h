#ifndef MYLANG_IF_STMT_H
#define MYLANG_IF_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class IfStmt : public Stmt
{
public:
    // 'then_branch' is expected to be a compound statement,
    // but 'else_branch' could be either nullptr,
    // if statement ('else-if' chaining),
    // or a compound statement (final 'else' branch)
    IfStmt(
        std::shared_ptr<Expr> condition,
        std::shared_ptr<Stmt> then_branch,
        std::shared_ptr<Stmt> else_branch
    );

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

private:
    std::shared_ptr<Expr> m_condition;
    std::shared_ptr<Stmt> m_then_branch;
    std::shared_ptr<Stmt> m_else_branch;
};

} // namespace mylang


#endif // MYLANG_IF_STMT_H
