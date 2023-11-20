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

    virtual void PreorderVisit(Module* node) override;
    virtual void PreorderVisit(FuncDecl* node) override;
    virtual void PreorderVisit(StructDecl* node) override;

    virtual void PreorderVisit(CompoundStmt* node) override {};
    virtual void PreorderVisit(IfStmt* node) override {};
    virtual void PreorderVisit(ForStmt* node) override {};
    virtual void PreorderVisit(WhileStmt* node) override {};
    virtual void PreorderVisit(JumpStmt* node) override {};
    virtual void PreorderVisit(VarDeclStmt* node) override {};
    virtual void PreorderVisit(ExprStmt* node) override {};

    virtual void PreorderVisit(VarInitExpr* node) override {};
    virtual void PreorderVisit(VarInitList* node) override {};

    virtual void PreorderVisit(ArrayAccessExpr* node) override {};
    virtual void PreorderVisit(BinaryExpr* node) override {};
    virtual void PreorderVisit(FuncCallExpr* node) override {};
    virtual void PreorderVisit(Identifier* node) override {};
    virtual void PreorderVisit(Literal* node) override {};
    virtual void PreorderVisit(MemberAccessExpr* node) override {};
    virtual void PreorderVisit(PostfixExpr* node) override {};
    virtual void PreorderVisit(PrefixExpr* node) override {};

private:
    ProgramEnvironment& m_environment;
    std::string_view m_module_name;
};

} // namespace mylang

#endif // MYLANG_GLOBAL_SYMBOL_SCANNER_H
