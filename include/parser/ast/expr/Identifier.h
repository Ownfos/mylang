#ifndef MYLANG_IDENTIFIER_H
#define MYLANG_IDENTIFIER_H

#include "parser/ast/expr/Expr.h"

namespace mylang
{

class Identifier : public Expr
{
public:
    Identifier(const Token& id);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;
    virtual const SourcePos& StartPos() const override;
    virtual std::string ToString() const override;

private:
    Token m_id;
};

} // namespace mylang


#endif // MYLANG_IDENTIFIER_H
