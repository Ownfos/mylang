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
    virtual const SourcePos& StartPos() const override;

    virtual const Token& Name() const override;
    virtual const Type& DeclType() const override;
    virtual bool ShouldExport() const override;

    const std::vector<MemberVariable>& Members() const;

private:
    bool m_should_export;
    Token m_name;
    std::vector<MemberVariable> m_members;

    // The type of struct itself.
    Type m_type;
};

} // namespace mylang

#endif // MYLANG_STRUCT_DECL_H
