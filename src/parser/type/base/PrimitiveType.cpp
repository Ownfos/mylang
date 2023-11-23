#include "parser/type/base/PrimitiveType.h"

namespace mylang
{

PrimitiveType::PrimitiveType(const Token& type)
    : m_type(type)
{}

std::string PrimitiveType::ToString() const
{
    return m_type.lexeme;
}

bool PrimitiveType::IsValid(
    ProgramEnvironment& environment,
    std::string_view context_module_name
) const
{
    // Primitive types are always valid.
    return true;
}

} // namespace mylang
