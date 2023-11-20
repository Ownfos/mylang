#ifndef MYLANG_I_BASE_TYPE_H
#define MYLANG_I_BASE_TYPE_H

#include <string>

namespace mylang
{

class IBaseType
{
public:
    virtual ~IBaseType() = default;

    virtual std::string ToString() const = 0;
    virtual bool IsStructType() const = 0;
};

} // namespace mylang

#endif // MYLANG_I_BASE_TYPE_H