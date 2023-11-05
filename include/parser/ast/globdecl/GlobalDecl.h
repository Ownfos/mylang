#ifndef MYLANG_GLOBAL_DECL_H
#define MYLANG_GLOBAL_DECL_H

#include "parser/ast/IAbstractSyntaxTree.h"
#include "lexer/Token.h"

namespace mylang
{

class GlobalDecl : public IAbstractSyntaxTree
{
public:
    GlobalDecl(bool should_export, const Token& name);

    bool ShouldExport() const;
    const Token& Name() const;

protected:
    bool m_should_export;
    Token m_name;
};

} // namespace mylang

#endif // MYLANG_GLOBAL_DECL_H
