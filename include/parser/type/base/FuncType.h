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
    FuncType(const std::vector<ParamType>& param_types, std::optional<Type> return_type);

    virtual std::string ToString() const override;
    virtual bool IsStructType() const override;

private:
    std::vector<ParamType> m_param_types;
    std::optional<Type> m_return_type;
};

} // namespace mylang


#endif // MYLANG_FUNC_TYPE_H
