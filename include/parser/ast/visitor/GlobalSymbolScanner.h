#ifndef MYLANG_GLOBAL_SYMBOL_SCANNER_H
#define MYLANG_GLOBAL_SYMBOL_SCANNER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/ProgramEnvironment.h"

namespace mylang
{

// Traverses an AST to collect global declarations and
// adds those symbols to the corresponding module's symbol table.
class GlobalSymbolScanner : public IAbstractSyntaxTreeVisitor
{
public:
    GlobalSymbolScanner(ProgramEnvironment& environment);

    virtual void Visit(Module* node) override;
    virtual void Visit(FuncDecl* node) override;
    virtual void Visit(StructDecl* node) override;

    virtual void Visit(CompoundStmt* node) override {};
    virtual void Visit(IfStmt* node) override {};
    virtual void Visit(ForStmt* node) override {};
    virtual void Visit(WhileStmt* node) override {};
    virtual void Visit(JumpStmt* node) override {};
    virtual void Visit(VarDeclStmt* node) override {};
    virtual void Visit(ExprStmt* node) override {};

    virtual void Visit(VarInitExpr* node) override {};
    virtual void Visit(VarInitList* node) override {};

    virtual void Visit(ArrayAccessExpr* node) override {};
    virtual void Visit(BinaryExpr* node) override {};
    virtual void Visit(FuncCallExpr* node) override {};
    virtual void Visit(Identifier* node) override {};
    virtual void Visit(Literal* node) override {};
    virtual void Visit(MemberAccessExpr* node) override {};
    virtual void Visit(PostfixExpr* node) override {};
    virtual void Visit(PrefixExpr* node) override {};

private:
    ProgramEnvironment& m_environment;
    std::string_view m_module_name;
};

} // namespace mylang

#endif // MYLANG_GLOBAL_SYMBOL_SCANNER_H
