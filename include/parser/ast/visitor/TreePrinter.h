#ifndef MYLANG_TREE_PRINTER_H
#define MYLANG_TREE_PRINTER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <ostream>

namespace mylang
{

class TreePrinter : public IAbstractSyntaxTreeVisitor
{
public:
    TreePrinter(std::ostream& output_stream, bool verbose = true, int indent_spaces = 4);

    void IncreaseDepth();
    void DecreaseDepth();

    // Preorder traversal
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

    // Postorder traversal
    virtual void PostorderVisit(Module* node) override;
    virtual void PostorderVisit(FuncDecl* node) override;
    virtual void PostorderVisit(StructDecl* node) override;

    virtual void PostorderVisit(CompoundStmt* node) override;
    virtual void PostorderVisit(IfStmt* node) override;
    virtual void PostorderVisit(ForStmt* node) override;
    virtual void PostorderVisit(WhileStmt* node) override;
    virtual void PostorderVisit(JumpStmt* node) override;
    virtual void PostorderVisit(VarDeclStmt* node) override;
    virtual void PostorderVisit(ExprStmt* node) override;

    virtual void PostorderVisit(VarInitExpr* node) override;
    virtual void PostorderVisit(VarInitList* node) override;

    virtual void PostorderVisit(ArrayAccessExpr* node) override;
    virtual void PostorderVisit(BinaryExpr* node) override;
    virtual void PostorderVisit(FuncCallExpr* node) override;
    virtual void PostorderVisit(MemberAccessExpr* node) override;
    virtual void PostorderVisit(PostfixExpr* node) override;
    virtual void PostorderVisit(PrefixExpr* node) override;

private:
    void Indent();

    std::ostream& m_output_stream;
    bool m_verbose;
    int m_indent_spaces;
    int m_indent_level = 0;
};

} // namespace mylang

#endif // MYLANG_TREE_PRINTER_H
