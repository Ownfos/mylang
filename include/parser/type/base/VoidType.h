#ifndef MYLANG_VOID_TYPE_H
#define MYLANG_VOID_TYPE_H

#include "parser/type/base/IBaseType.h"

namespace mylang
{

// Since there is no explicit "void" keyword,
// this type is dedicated solely for void return type of functions.
class VoidType : public IBaseType
{
public:
    virtual std::string ToString() const override;
    virtual bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const override;
};

} // namespace mylang

#endif // MYLANG_VOID_TYPE_H
