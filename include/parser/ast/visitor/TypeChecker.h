#ifndef MYLANG_TYPE_CHECKER_H
#define MYLANG_TYPE_CHECKER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/type/Type.h"
#include <map>
#include <stack>

namespace mylang
{

class IAbstractSyntaxTree;

class TypeChecker : public IAbstractSyntaxTreeVisitor
{
public:
    virtual void PreorderVisit(Module* node) override;
    virtual void PreorderVisit(FuncDecl* node) override;
    virtual void PreorderVisit(StructDecl* node) override;

    virtual void PreorderVisit(CompoundStmt* node) override;
    virtual void PreorderVisit(IfStmt* node) override;
    virtual void PreorderVisit(ForStmt* node) override;
    virtual void PreorderVisit(WhileStmt* node) override;
    virtual void PreorderVisit(JumpStmt* node) override;
    virtual void PreorderVisit(VarDeclStmt* node) override;
    virtual void PreorderVisit(ExprStmt* node) override;

    virtual void PreorderVisit(VarInitExpr* node) override;
    virtual void PreorderVisit(VarInitList* node) override;

    virtual void PreorderVisit(ArrayAccessExpr* node) override;
    virtual void PreorderVisit(BinaryExpr* node) override;
    virtual void PreorderVisit(FuncCallExpr* node) override;
    virtual void PreorderVisit(Identifier* node) override;
    virtual void PreorderVisit(Literal* node) override;
    virtual void PreorderVisit(MemberAccessExpr* node) override;
    virtual void PreorderVisit(PostfixExpr* node) override;
    virtual void PreorderVisit(PrefixExpr* node) override;

private:
    std::stack<IAbstractSyntaxTree*> m_call_stack;
    std::map<IAbstractSyntaxTree*, Type> m_type_dict;
};

} // namespace mylang

#endif // MYLANG_TYPE_CHECKER_H
