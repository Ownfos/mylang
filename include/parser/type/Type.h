#ifndef MYLANG_TYPE_DECL_H
#define MYLANG_TYPE_DECL_H

#include "parser/type/IBaseType.h"
#include <vector>
#include <memory>

namespace mylang
{

// Type is a base class of all types, including functions and structs.
class Type
{
public:
    Type(std::shared_ptr<IBaseType> base_type, const std::vector<int>& array_sizes);
    
    const IBaseType* BaseType() const;
    const std::vector<int>& ArraySize() const;

    std::string ToString() const;

    // Create a resulting type of an array access operator "[]".
    // ex) i32[10][20] -> i32[20]
    // ex) bool[5] -> bool
    static Type RemoveLeftmostArrayDim(const Type& array_type);

protected:
    std::shared_ptr<IBaseType> m_base_type;
    std::vector<int> m_array_sizes;
};

} // namespace mylang

#endif // MYLANG_TYPE_DECL_H
