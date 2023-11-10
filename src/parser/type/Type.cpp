#include "parser/type/Type.h"
#include <format>

namespace mylang
{

Type::Type(std::shared_ptr<IBaseType> base_type, const std::vector<int>& array_sizes)
    : m_base_type(base_type), m_array_sizes(array_sizes)
{}

const IBaseType* Type::BaseType() const
{
    return m_base_type.get();
}

const std::vector<int>& Type::ArraySize() const
{
    return m_array_sizes;
}

std::string Type::ToString() const
{
    // Get the base type description.
    auto type_str = m_base_type->ToString();

    // Append array size information.
    for (auto size : m_array_sizes)
    {
        type_str.append(std::format("[{}]", size));
    }
    
    return type_str;
}

Type Type::RemoveLeftmostArrayDim(const Type& array_type)
{
    // Copy type instance.
    auto result = array_type;

    // Remove the first dimension size.
    result.m_array_sizes.erase(result.m_array_sizes.begin());

    return result;
}

} // namespace mylang
