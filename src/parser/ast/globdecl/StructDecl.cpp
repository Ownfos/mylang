#include "parser/ast/globdecl/StructDecl.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/type/base/StructType.h"

namespace mylang
{

StructDecl::StructDecl(bool should_export, const Token& name, const std::vector<MemberVariable>& members)
    : m_should_export(should_export)
    , m_name(name)
    , m_members(members)
    , m_type(std::make_shared<StructType>(name))
{}

void StructDecl::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    visitor->PostorderVisit(this);
}

const Token& StructDecl::Name() const
{
    return m_name;
}

const Type& StructDecl::DeclType() const
{
    return m_type;
}

bool StructDecl::ShouldExport() const
{
    return m_should_export;
}

const std::vector<MemberVariable>& StructDecl::Members() const
{
    return m_members;
}

} // namespace mylang
