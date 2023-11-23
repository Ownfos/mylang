#ifndef MYLANG_I_BASE_TYPE_H
#define MYLANG_I_BASE_TYPE_H

#include <string>

namespace mylang
{

class ProgramEnvironment;

class IBaseType
{
public:
    virtual ~IBaseType() = default;

    virtual std::string ToString() const = 0;

    // Same type specifier can be valid or invalid depending
    // on the context where it was used.
    //
    // To be specific, validity of a struct type can be checked
    // only after module dependecy and global symbol scanning is done.
    //
    // For example, in order to say that "x: cat;" is a valid declaration,
    // we need to confirm that "cat" is visible in this module's context
    // and the symbol was indeed declared as a StructDecl.
    virtual bool IsValid(
        ProgramEnvironment& environment,
        std::string_view context_module_name
    ) const = 0;
};

} // namespace mylang

#endif // MYLANG_I_BASE_TYPE_H