#include "parser/ast/globdecl/GlobalDecl.h"

namespace mylang
{

GlobalDecl::GlobalDecl(bool should_export, const Token& name)
    : m_should_export(should_export), m_name(name)
{}

bool GlobalDecl::ShouldExport() const
{
    return m_should_export;
}

const Token& GlobalDecl::Name() const
{
    return m_name;
}

} // namespace mylang
