#include "parser/type/PrimitiveType.h"

namespace mylang
{

PrimitiveType::PrimitiveType(const Token& type)
    : m_type(type)
{}

std::string PrimitiveType::ToString() const
{
    return m_type.lexeme;
}

} // namespace mylang
