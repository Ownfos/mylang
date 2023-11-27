#include "parser/type/base/FuncType.h"
#include <format>
#include <sstream>

namespace mylang
{

std::string_view UsageString(ParamUsage usage)
{
    if (usage == ParamUsage::In)
    {
        return "in";
    }
    else if (usage == ParamUsage::Out)
    {
        return "out";
    }
    else
    {
        return "inout";
    }
}

std::string ParamType::ToString() const
{
    return std::format("{} {}", UsageString(usage), type.ToString());
}


FuncType::FuncType(const std::vector<ParamType>& param_types, const Type& return_type)
    : m_param_types(param_types)
    , m_return_type(return_type)
{}

std::string FuncType::ToString() const
{
    auto str_builder = std::ostringstream();
    str_builder << "[(";

    // Parameter type list
    for (int i = 0; i < m_param_types.size(); ++i)
    {
        // Add seperator for second to last paramters.
        if (i > 0)
        {
            str_builder << ", ";
        }

        str_builder << m_param_types[i].ToString();
    }
    str_builder << ")";

    // Return type
    if (m_return_type != CreateVoidType())
    {
        str_builder << " -> " << m_return_type.ToString();
    }
    
    str_builder << "]";
    return str_builder.str();
}

bool FuncType::IsValid(
    ProgramEnvironment& environment,
    std::string_view context_module_name
) const
{
    // If return type is invalid, FuncType is also invalid.
    if (!m_return_type.IsValid(environment, context_module_name))
    {
        return false;
    }

    // If any paramter type is invalid, FuncType is also invalid.
    for (const auto& [type, usage] : m_param_types)
    {
        if (!type.IsValid(environment, context_module_name))
        {
            return false;
        }
    }

    // Everything is valid!
    return true;
}

const std::vector<ParamType>& FuncType::ParamTypes() const
{
    return m_param_types;
}

const Type& FuncType::ReturnType() const
{
    return m_return_type;
}

} // namespace mylang
