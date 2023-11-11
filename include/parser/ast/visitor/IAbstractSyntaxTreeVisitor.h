#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

// TODO: add concrete class of tree nodes like below
class Program;
class FuncDecl;
class StructDecl;
class CompoundStmt;

class ArrayAccessExpr;
class BinaryExpr;
class FuncCallExpr;
class Identifier;
class Literal;
class MemberAccessExpr;
class PostfixExpr;
class PrefixExpr;

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

    virtual void Visit(ArrayAccessExpr* node) = 0;
    virtual void Visit(BinaryExpr* node) = 0;
    virtual void Visit(FuncCallExpr* node) = 0;
    virtual void Visit(Identifier* node) = 0;
    virtual void Visit(Literal* node) = 0;
    virtual void Visit(MemberAccessExpr* node) = 0;
    virtual void Visit(PostfixExpr* node) = 0;
    virtual void Visit(PrefixExpr* node) = 0;
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
