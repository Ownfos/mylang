#ifndef MYLANG_STRUCT_TYPE_H
#define MYLANG_STRUCT_TYPE_H

#include "parser/type/base/IBaseType.h"
#include "lexer/Token.h"

namespace mylang
{

// A base type class for user-defined struct types.
class StructType : public IBaseType
{
public:
    StructType(const Token& type);

    virtual std::string ToString() const override;
    virtual bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const override;

private:
    Token m_type;
};

} // namespace mylang

#endif // MYLANG_STRUCT_TYPE_H
