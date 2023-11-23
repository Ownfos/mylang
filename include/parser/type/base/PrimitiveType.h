#ifndef MYLANG_PRIMITIVE_TYPE_H
#define MYLANG_PRIMITIVE_TYPE_H

#include "parser/type/base/IBaseType.h"
#include "lexer/Token.h"

namespace mylang
{

// A base type class for primitive types (i32, f32, bool, str).
class PrimitiveType : public IBaseType
{
public:
    PrimitiveType(const Token& type);

    virtual std::string ToString() const override;
    virtual bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const override;

private:
    Token m_type;
};

} // namespace mylang

#endif // MYLANG_PRIMITIVE_TYPE_H
