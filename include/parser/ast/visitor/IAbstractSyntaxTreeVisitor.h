#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

class Module;

class Parameter;
class FuncDecl;
class StructDecl;

class CompoundStmt;
class IfStmt;
class ForStmt;
class WhileStmt;
class JumpStmt;
class VarDeclStmt;
class ExprStmt;

class VarInitExpr;
class VarInitList;

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
    
    //  traversal
    virtual void Visit(Module* node) {};

    virtual void Visit(Parameter* node) {};
    virtual void Visit(FuncDecl* node) {};
    virtual void Visit(StructDecl* node) {};

    virtual void Visit(CompoundStmt* node) {};
    virtual void Visit(IfStmt* node) {};
    virtual void Visit(ForStmt* node) {};
    virtual void Visit(WhileStmt* node) {};
    virtual void Visit(JumpStmt* node) {};
    virtual void Visit(VarDeclStmt* node) {};
    virtual void Visit(ExprStmt* node) {};

    virtual void Visit(VarInitExpr* node) {};
    virtual void Visit(VarInitList* node) {};

    virtual void Visit(ArrayAccessExpr* node) {};
    virtual void Visit(BinaryExpr* node) {};
    virtual void Visit(FuncCallExpr* node) {};
    virtual void Visit(Identifier* node) {};
    virtual void Visit(Literal* node) {};
    virtual void Visit(MemberAccessExpr* node) {};
    virtual void Visit(PostfixExpr* node) {};
    virtual void Visit(PrefixExpr* node) {};
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
