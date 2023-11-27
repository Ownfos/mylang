#ifndef MYLANG_FUNC_TYPE_H
#define MYLANG_FUNC_TYPE_H

#include "parser/type/base/IBaseType.h"
#include "parser/type/Type.h"
#include <vector>
#include <optional>

namespace mylang
{

// ParamUsage describes how a function parameter should be used.
// - In: should not be modified, but used at least once.
// - Out: should be modified at least once, but not used.
// - InOut: should be used and modified at least once.
enum class ParamUsage
{
    In, // Read-only
    Out, // Write-only
    InOut // Read-and-write
};

struct ParamType
{
    Type type;
    ParamUsage usage;

    std::string ToString() const;
};

// A base type class for callables (functions and functors)
class FuncType : public IBaseType
{
public:
    FuncType(const std::vector<ParamType>& param_types, const Type& return_type);

    virtual std::string ToString() const override;
    virtual bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const override;

    const std::vector<ParamType>& ParamTypes() const;
    const Type& ReturnType() const;

private:
    std::vector<ParamType> m_param_types;
    Type m_return_type;
};

} // namespace mylang


#endif // MYLANG_FUNC_TYPE_H
