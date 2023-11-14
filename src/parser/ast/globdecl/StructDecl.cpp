#include "parser/ast/globdecl/StructDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

StructDecl::StructDecl(bool should_export, const Token& name, const std::vector<MemberVariable>& members)
    : GlobalDecl(should_export, name)
    , m_members(members)
{}

void StructDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const std::vector<MemberVariable>& StructDecl::Members() const
{
    return m_members;
}

} // namespace mylang
