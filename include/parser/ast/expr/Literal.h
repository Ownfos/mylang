#ifndef MYLANG_LITERAL_H
#define MYLANG_LITERAL_H

#include "parser/ast/expr/Expr.h"

namespace mylang
{

class Literal : public Expr
{
public:
    Literal(const Token& literal);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual std::string ToString() const override;

private:
    Token m_literal;
};

} // namespace mylang


#endif // MYLANG_LITERAL_H
