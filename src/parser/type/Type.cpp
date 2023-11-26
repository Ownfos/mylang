#include "parser/type/Type.h"
#include "parser/type/base/PrimitiveType.h"
#include "parser/ProgramEnvironment.h"
#include <format>

namespace mylang
{

Type CreatePrimiveType(TokenType type)
{
    if (type != TokenType::IntType &&
        type != TokenType::FloatType &&
        type != TokenType::BoolType &&
        type != TokenType::StringType)
    {
        throw std::exception("valid primitive types are f32, i32, bool, and str");
    }

    auto token = Token{
        .type = type,
        .lexeme = TokenTypeName(type)
    };
    return Type(std::make_shared<PrimitiveType>(token));
}

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

bool Type::IsArray() const
{
    return !m_array_sizes.empty();
}

int Type::NumDimensions() const
{
    return static_cast<int>(m_array_sizes.size());
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
    // Do not allow mixing arrays with different number of dimensions.
    if (m_array_sizes.size() != other.m_array_sizes.size())
    {
        throw std::exception("MergeArrayDim() is only allowed on two arrays with same number of dimensions");
    }

    for (int i = 0; i < m_array_sizes.size(); ++i)
    {
        m_array_sizes[i] = std::max(m_array_sizes[i], other.m_array_sizes[i]);
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
