#ifndef MYLANG_TYPE_DECL_H
#define MYLANG_TYPE_DECL_H

#include <string>

namespace mylang
{

// Type is a base class of all types, including functions and arrays.
class Type
{
public:
    virtual ~Type() = default;

    virtual std::string ToString() const = 0;
};

} // namespace mylang

#endif // MYLANG_TYPE_DECL_H
