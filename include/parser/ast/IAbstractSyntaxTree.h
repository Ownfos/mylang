#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_H

#include "file/SourcePos.h"

namespace mylang
{

class IAbstractSyntaxTreeVisitor;

class IAbstractSyntaxTree
{
public:
    virtual ~IAbstractSyntaxTree() = default;

    // Implement visitor pattern
    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) = 0;

    // Returns the starting source location of this node's region.
    // For example, module declaration should return its name token location.
    virtual const SourcePos& StartPos() const = 0;
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_H
