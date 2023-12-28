#include "parser/type/base/VoidType.h"

namespace mylang
{

std::string VoidType::ToString() const
{
    return "void";
}

std::string VoidType::ToCppString() const
{
    return "void";
}

bool VoidType::IsValid(
    ProgramEnvironment& environment,
    std::string_view context_module_name
) const
{
    return true;
}

} // namespace mylang
