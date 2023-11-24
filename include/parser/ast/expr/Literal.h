#ifndef MYLANG_LITERAL_H
#define MYLANG_LITERAL_H

#include "parser/ast/expr/Expr.h"
#include "parser/type/Type.h"

namespace mylang
{

class Literal : public Expr
{
public:
    Literal(const Token& literal);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual std::string ToString() const override;

    const Type& DeclType() const;

private:
    Token m_literal;
    Type m_decl_type;
};

} // namespace mylang


#endif // MYLANG_LITERAL_H
