#ifndef MYLANG_VAR_DECL_STMT_H
#define MYLANG_VAR_DECL_STMT_H

#include "parser/ast/varinit/VarInit.h"
#include "parser/ast/stmt/Stmt.h"
#include "parser/ast/expr/Expr.h"
#include "parser/ast/Decl.h"
#include <memory>

namespace mylang
{

class VarDeclStmt : public Stmt, public Decl
{
public:
    VarDeclStmt(const Token& name, const Type& type, std::shared_ptr<VarInit> initializer);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    virtual const Token& Name() const;
    virtual const Type& DeclType() const override;

private:
    Token m_name;
    Type m_type;
    std::shared_ptr<VarInit> m_initializer;
};

} // namespace mylang


#endif // MYLANG_VAR_DECL_STMT_H
