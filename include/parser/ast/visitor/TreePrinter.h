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

    virtual void IncreaseDepth() override;
    virtual void DecreaseDepth() override;

    virtual void Visit(Module* node) override;
    virtual void Visit(FuncDecl* node) override;
    virtual void Visit(StructDecl* node) override;

    virtual void Visit(CompoundStmt* node) override;
    virtual void Visit(IfStmt* node) override;
    virtual void Visit(ForStmt* node) override;
    virtual void Visit(WhileStmt* node) override;
    virtual void Visit(JumpStmt* node) override;
    virtual void Visit(VarDeclStmt* node) override;
    virtual void Visit(ExprStmt* node) override;

    virtual void Visit(VarInitExpr* node) override;
    virtual void Visit(VarInitList* node) override;

    virtual void Visit(ArrayAccessExpr* node) override;
    virtual void Visit(BinaryExpr* node) override;
    virtual void Visit(FuncCallExpr* node) override;
    virtual void Visit(Identifier* node) override;
    virtual void Visit(Literal* node) override;
    virtual void Visit(MemberAccessExpr* node) override;
    virtual void Visit(PostfixExpr* node) override;
    virtual void Visit(PrefixExpr* node) override;

private:
    void Indent();

    std::ostream& m_output_stream;
    bool m_verbose;
    int m_indent_spaces;
    int m_indent_level = 0;
};

} // namespace mylang

#endif // MYLANG_TREE_PRINTER_H
