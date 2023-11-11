#ifndef MYLANG_VAR_DECL_STMT_H
#define MYLANG_VAR_DECL_STMT_H

#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include "parser/type/Type.h"
#include <memory>

namespace mylang
{

class VarDeclStmt : public Stmt
{
public:
    VarDeclStmt(const Token& name, const Type& type, std::shared_ptr<Expr> expr);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    const Token& Name() const;
    const Type& TypeSpecifier() const;

private:
    Token m_name;
    Type m_type;
    std::shared_ptr<Expr> m_expr;
};

} // namespace mylang


#endif // MYLANG_VAR_DECL_STMT_H
