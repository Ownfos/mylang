#include "parser/type/base/StructType.h"

namespace mylang
{

StructType::StructType(const Token& type)
    : m_type(type)
{}

std::string StructType::ToString() const
{
    return m_type.lexeme;
}

} // namespace mylang
