#ifndef MYLANG_JUMP_STMT_H
#define MYLANG_JUMP_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include "lexer/Token.h"
#include <memory>

namespace mylang
{

class JumpStmt : public Stmt
{
public:
    // 'jump_type' should be either "return", "break", or "continue".
    // 'expr' could be nullptr, if a function has no return type.
    JumpStmt(const Token& jump_type, std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;

    const Token& JumpType() const;
    const Expr* ReturnValueExpr() const;

private:
    Token m_jump_type;
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang


#endif // MYLANG_JUMP_STMT_H
