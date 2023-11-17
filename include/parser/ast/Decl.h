#ifndef MYLANG_DECL_H
#define MYLANG_DECL_H

#include "parser/ast/IAbstractSyntaxTree.h"
#include "parser/type/Type.h"
#include "lexer/Token.h"

namespace mylang
{

// A base class of all symbol declarations: function, struct, and variable.
class Decl : public IAbstractSyntaxTree
{
public:
    virtual const Token& Name() const = 0;
    virtual const Type& DeclType() const = 0;
};

} // namespace mylang


#endif // MYLANG_DECL_H
