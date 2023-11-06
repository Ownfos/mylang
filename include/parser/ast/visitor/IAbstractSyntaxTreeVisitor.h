#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

// TODO: add concrete class of tree nodes like below
class Program;
class FuncDecl;
class StructDecl;
class CompoundStmt;

class IAbstractSyntaxTreeVisitor
{
public:
    virtual ~IAbstractSyntaxTreeVisitor() = default;

    virtual void IncreaseDepth() {};
    virtual void DecreaseDepth() {};

    // TODO: add concrete class of tree nodes like below
    virtual void Visit(Program* node) = 0;
    virtual void Visit(FuncDecl* node) = 0;
    virtual void Visit(StructDecl* node) = 0;
    virtual void Visit(CompoundStmt* node) = 0;
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
