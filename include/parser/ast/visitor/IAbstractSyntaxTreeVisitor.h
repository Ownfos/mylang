#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

class Program;
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

    virtual void IncreaseDepth() {};
    virtual void DecreaseDepth() {};

    virtual void Visit(Program* node) = 0;
    virtual void Visit(FuncDecl* node) = 0;
    virtual void Visit(StructDecl* node) = 0;

    virtual void Visit(CompoundStmt* node) = 0;
    virtual void Visit(IfStmt* node) = 0;
    virtual void Visit(ForStmt* node) = 0;
    virtual void Visit(WhileStmt* node) = 0;
    virtual void Visit(JumpStmt* node) = 0;
    virtual void Visit(VarDeclStmt* node) = 0;
    virtual void Visit(ExprStmt* node) = 0;

    virtual void Visit(VarInitExpr* node) = 0;
    virtual void Visit(VarInitList* node) = 0;

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
