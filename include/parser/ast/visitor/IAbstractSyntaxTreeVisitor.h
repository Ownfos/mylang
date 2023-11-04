#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

// TODO: add concrete class of tree nodes like below
// class ConcreteClass;

class IAbstractSyntaxTreeVisitor
{
public:
    virtual ~IAbstractSyntaxTreeVisitor() = default;

    // TODO: add concrete class of tree nodes like below
    // virtual void Visit(ConcreteClass* node) = 0;
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
