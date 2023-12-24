#ifndef MYLANG_JUMP_STMT_USAGE_CHECKER_H
#define MYLANG_JUMP_STMT_USAGE_CHECKER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

// Checks if break or continue is used only inside a loop.
//
// Since ExprStmt and VarDeclStmt cannot contain JumpStmt
// and any JumpStmt in WhileStmt and ForStmt is valid,
// we only need to traverse and inspect IfStmt and CompoundStmt.
//
// On valid AST, we should NOT encounter any break or continue.
class JumpStmtUsageChecker : public IAbstractSyntaxTreeVisitor
{
public:
    virtual void Visit(Module* node) override;
    virtual void Visit(FuncDecl* node) override;

    virtual void Visit(IfStmt* node) override;
    virtual void Visit(CompoundStmt* node) override;
    virtual void Visit(JumpStmt* node) override;
};

} // namespace mylang

#endif // MYLANG_JUMP_STMT_USAGE_CHECKER_H
