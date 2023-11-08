#ifndef MYLANG_PRIMITIVE_TYPE_H
#define MYLANG_PRIMITIVE_TYPE_H

#include "parser/type/Type.h"
#include "lexer/Token.h"

namespace mylang
{

class PrimitiveType : public Type
{
public:
    PrimitiveType(const Token& type);

    virtual std::string ToString() const override;

private:
    Token m_type;
};

} // namespace mylang

#endif // MYLANG_PRIMITIVE_TYPE_H
