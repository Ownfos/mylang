#ifndef MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
#define MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H

namespace mylang
{

class Module;
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
    
    // Preorder traversal
    virtual void PreorderVisit(Module* node) {};
    virtual void PreorderVisit(FuncDecl* node) {};
    virtual void PreorderVisit(StructDecl* node) {};

    virtual void PreorderVisit(CompoundStmt* node) {};
    virtual void PreorderVisit(IfStmt* node) {};
    virtual void PreorderVisit(ForStmt* node) {};
    virtual void PreorderVisit(WhileStmt* node) {};
    virtual void PreorderVisit(JumpStmt* node) {};
    virtual void PreorderVisit(VarDeclStmt* node) {};
    virtual void PreorderVisit(ExprStmt* node) {};

    virtual void PreorderVisit(VarInitExpr* node) {};
    virtual void PreorderVisit(VarInitList* node) {};

    virtual void PreorderVisit(ArrayAccessExpr* node) {};
    virtual void PreorderVisit(BinaryExpr* node) {};
    virtual void PreorderVisit(FuncCallExpr* node) {};
    virtual void PreorderVisit(Identifier* node) {};
    virtual void PreorderVisit(Literal* node) {};
    virtual void PreorderVisit(MemberAccessExpr* node) {};
    virtual void PreorderVisit(PostfixExpr* node) {};
    virtual void PreorderVisit(PrefixExpr* node) {};

    // Postorder traversal
    virtual void PostorderVisit(Module* node) {};
    virtual void PostorderVisit(FuncDecl* node) {};
    virtual void PostorderVisit(StructDecl* node) {};

    virtual void PostorderVisit(CompoundStmt* node) {};
    virtual void PostorderVisit(IfStmt* node) {};
    virtual void PostorderVisit(ForStmt* node) {};
    virtual void PostorderVisit(WhileStmt* node) {};
    virtual void PostorderVisit(JumpStmt* node) {};
    virtual void PostorderVisit(VarDeclStmt* node) {};
    virtual void PostorderVisit(ExprStmt* node) {};

    virtual void PostorderVisit(VarInitExpr* node) {};
    virtual void PostorderVisit(VarInitList* node) {};

    virtual void PostorderVisit(ArrayAccessExpr* node) {};
    virtual void PostorderVisit(BinaryExpr* node) {};
    virtual void PostorderVisit(FuncCallExpr* node) {};
    virtual void PostorderVisit(Identifier* node) {};
    virtual void PostorderVisit(Literal* node) {};
    virtual void PostorderVisit(MemberAccessExpr* node) {};
    virtual void PostorderVisit(PostfixExpr* node) {};
    virtual void PostorderVisit(PrefixExpr* node) {};
};

} // namespace mylang

#endif // MYLANG_I_ABSTRACT_SYNTAX_TREE_VISITOR_H
