#ifndef MYLANG_WHILE_STMT_H
#define MYLANG_WHILE_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include <memory>

namespace mylang
{

class WhileStmt : public Stmt
{
public:
    // 'body' is expected to be a compound statement.
    WhileStmt(std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> body);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

private:
    std::shared_ptr<Expr> m_condition;
    std::shared_ptr<Stmt> m_body;
};

} // namespace mylang


#endif // MYLANG_WHILE_STMT_H
