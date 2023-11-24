#ifndef MYLANG_TYPE_H
#define MYLANG_TYPE_H

#include "parser/type/base/IBaseType.h"
#include <vector>
#include <memory>

namespace mylang
{

// Type is a tuple of base type and array info.
// It provides turning every base type into array of base type,
// and some compile-type type manipulations such as removing an array dimension.
class Type
{
public:
    Type(std::shared_ptr<IBaseType> base_type, const std::vector<int>& array_sizes = {});

    const IBaseType* BaseType() const;
    const std::vector<int>& ArraySize() const;

    // Wrapper functions for underlying base type.
    std::string ToString() const;
    bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const;

    // Create a resulting type of an array access operator "[]".
    // ex) i32[10][20] -> i32[20]
    // ex) bool[5] -> bool
    static Type RemoveLeftmostArrayDim(const Type& array_type);

    bool operator==(const Type& other) const;
    bool operator!=(const Type& other) const;

protected:
    std::shared_ptr<IBaseType> m_base_type;
    std::vector<int> m_array_sizes;
};

} // namespace mylang

#endif // MYLANG_TYPE_H
