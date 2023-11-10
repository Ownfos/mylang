#ifndef MYLANG_PRIMITIVE_TYPE_H
#define MYLANG_PRIMITIVE_TYPE_H

#include "parser/type/IBaseType.h"
#include "lexer/Token.h"

namespace mylang
{

// A base type class for i32, f32, bool, and str.
class PrimitiveType : public IBaseType
{
public:
    PrimitiveType(const Token& type);

    virtual std::string ToString() const override;

private:
    Token m_type;
};

} // namespace mylang

#endif // MYLANG_PRIMITIVE_TYPE_H
