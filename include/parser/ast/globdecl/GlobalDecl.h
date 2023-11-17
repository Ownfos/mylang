#ifndef MYLANG_GLOBAL_DECL_H
#define MYLANG_GLOBAL_DECL_H

#include "parser/ast/Decl.h"

namespace mylang
{

// Global declaration refers to declaration of symbols
// that can be exported to another module via "export" keyword.
class GlobalDecl : public Decl
{
public:
    virtual bool ShouldExport() const = 0;
};

} // namespace mylang

#endif // MYLANG_GLOBAL_DECL_H
