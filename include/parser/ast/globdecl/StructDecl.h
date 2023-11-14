#ifndef MYLANG_STRUCT_DECL_H
#define MYLANG_STRUCT_DECL_H

#include "parser/ast/globdecl/GlobalDecl.h"
#include "parser/type/Type.h"
#include <vector>

namespace mylang
{

struct MemberVariable
{
    Token name;
    Type type;
};

class StructDecl : public GlobalDecl
{
public:
    // 'should_export' is true iff "export" prefix was found.
    StructDecl(bool should_export, const Token& name, const std::vector<MemberVariable>& members);

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) override;

    const std::vector<MemberVariable>& Members() const;

private:
    std::vector<MemberVariable> m_members;
};

} // namespace mylang

#endif // MYLANG_STRUCT_DECL_H
