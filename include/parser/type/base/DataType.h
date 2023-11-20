#ifndef MYLANG_DATA_TYPE_H
#define MYLANG_DATA_TYPE_H

#include "parser/type/base/IBaseType.h"
#include "lexer/Token.h"

namespace mylang
{

// A base type class for primitive types (i32, f32, bool, str) and structs.
class DataType : public IBaseType
{
public:
    DataType(const Token& type);

    virtual std::string ToString() const override;
    virtual bool IsStructType() const override;

private:
    Token m_type;
};

} // namespace mylang

#endif // MYLANG_DATA_TYPE_H
