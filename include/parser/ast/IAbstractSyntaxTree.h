#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_H

namespace mylang
{

class IAbstractSyntaxTreeVisitor;

class IAbstractSyntaxTree
{
public:
    virtual ~IAbstractSyntaxTree() = default;

    virtual void Accept(IAbstractSyntaxTreeVisitor* visitor) = 0;
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_H
