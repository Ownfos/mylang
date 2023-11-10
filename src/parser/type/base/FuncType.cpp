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

FuncType::FuncType(const std::vector<ParamType>& param_types, std::optional<Type> return_type)
    : m_param_types(param_types), m_return_type(return_type)
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
    if (m_return_type)
    {
        str_builder << " -> " << m_return_type->ToString();
    }
    
    str_builder << "]";
    return str_builder.str();
}

} // namespace mylang
