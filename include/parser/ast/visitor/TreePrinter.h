#ifndef MYLANG_TREE_PRINTER_H
#define MYLANG_TREE_PRINTER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include <ostream>

namespace mylang
{

class TreePrinter : public IAbstractSyntaxTreeVisitor
{
public:
    TreePrinter(std::ostream& output_stream, int indent_spaces = 4);

    virtual void IncreaseDepth() override;
    virtual void DecreaseDepth() override;

    virtual void Visit(Program* node) override;
    virtual void Visit(FuncDecl* node) override;
    virtual void Visit(StructDecl* node) override;
    virtual void Visit(CompoundStmt* node) override;

private:
    void Indent();

    std::ostream& m_output_stream;
    int m_indent_spaces;
    int m_indent_level = 0;
};

} // namespace mylang

#endif // MYLANG_TREE_PRINTER_H
