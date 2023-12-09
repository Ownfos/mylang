#ifndef MYLANG_JUMP_STMT_USAGE_CHECKER_H
#define MYLANG_JUMP_STMT_USAGE_CHECKER_H

#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

// Checks if break or continue is used only inside a loop.
class JumpStmtUsageChecker : public IAbstractSyntaxTreeVisitor
{
public:
    virtual void PreorderVisit(ForStmt* node) override;
    virtual void PostorderVisit(ForStmt* node) override;
    virtual void PreorderVisit(WhileStmt* node) override;
    virtual void PostorderVisit(WhileStmt* node) override;
    virtual void PostorderVisit(JumpStmt* node) override;
    
private:
    // Increase or decrease loop nesting level counter.
    void EnterLoop();
    void ExitLoop();

    int m_loop_nesting_level = 0;
};

} // namespace mylang

#endif // MYLANG_JUMP_STMT_USAGE_CHECKER_H
