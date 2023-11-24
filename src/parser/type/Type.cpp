#include "parser/type/Type.h"
#include "parser/ProgramEnvironment.h"
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

bool Type::IsValid(
    ProgramEnvironment& environment,
    std::string_view context_module_name
) const
{
    // Make sure base type is good.
    if (!m_base_type->IsValid(environment, context_module_name))
    {
        return false;
    }

    // Now check the array sizes, which should be greater than 0.
    for (auto size : m_array_sizes)
    {
        if (size <= 0)
        {
            return false;
        }
    }

    // Everything is valid!
    return true;
}

void Type::RemoveLeftmostArrayDim()
{
    m_array_sizes.erase(m_array_sizes.begin());
}

void Type::AddLeftmostArrayDim(int dimension_size)
{
    m_array_sizes.insert(m_array_sizes.begin(), dimension_size);
}

void Type::MergeArrayDim(const Type& other)
{
    // If 'other' has higher dimension, copy the upper dimension sizes
    // ex)
    // 'this': [1], 'other': [3][2][1]
    // => i starts at (3 - 2 - 1) = 1, which points to 2
    // => AddLeftmostArrayDim(2)
    // => AddLeftmostArrayDim(3)
    auto my_dim = static_cast<int>(m_array_sizes.size());
    auto other_dim = static_cast<int>(other.m_array_sizes.size());
    for (int i = other_dim - my_dim - 1; i >= 0; --i)
    {
        my_dim++;
        AddLeftmostArrayDim(other.m_array_sizes[i]);
    }

    // Since 'other' can have lower dimension than 'this', loop runs w.r.t. 'other'.
    // Compare the rightmost dimension of each array, then move to the left one by one.
    // Any higher dimension that does not exist in 'other' does not require modification.
    // ex)
    // 'this': [3][2][1], 'other': [5][2]
    // => dim_offset = 1
    // => [1] vs [2] => [2]
    // => [2] vs [5] => [5]
    // => final result: [3][5][2]
    auto dim_offset = my_dim - other_dim;
    for (int i = 0; i < other.m_array_sizes.size(); ++i)
    {
        m_array_sizes[dim_offset + i] = std::max(m_array_sizes[dim_offset + i], other.m_array_sizes[i]);
    }
}

bool Type::operator==(const Type& other) const
{
    return ToString() == other.ToString();
}

bool Type::operator!=(const Type& other) const
{
    return !(*this == other);
}

} // namespace mylang
