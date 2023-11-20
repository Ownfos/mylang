#include "parser/type/base/DataType.h"

namespace mylang
{

DataType::DataType(const Token& type)
    : m_type(type)
{}

std::string DataType::ToString() const
{
    return m_type.lexeme;
}

bool DataType::IsStructType() const
{
    return m_type.type == TokenType::Identifier;
}

} // namespace mylang
