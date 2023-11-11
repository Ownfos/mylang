#ifndef MYLANG_EXPR_DECL_H
#define MYLANG_EXPR_DECL_H

#include "parser/ast/IAbstractSyntaxTree.h"
#include "lexer/Token.h"

namespace mylang
{

class Expr : public IAbstractSyntaxTree
{
public:
    virtual std::string ToString() const = 0;
};

} // namespace mylang

#endif // MYLANG_EXPR_DECL_H
