#include "parser/ast/visitor/JumpStmtUsageChecker.h"
#include "parser/ast/stmt/ForStmt.h"
#include "parser/ast/stmt/WhileStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/SemanticError.h"
#include <format>

namespace mylang
{

void JumpStmtUsageChecker::Visit(ForStmt* node)
{
    EnterLoop();
}

void JumpStmtUsageChecker::Visit(ForStmt* node)
{
    ExitLoop();
}

void JumpStmtUsageChecker::Visit(WhileStmt* node)
{
    EnterLoop();
}

void JumpStmtUsageChecker::Visit(WhileStmt* node)
{
    ExitLoop();
}

void JumpStmtUsageChecker::Visit(JumpStmt* node)
{
    // We only need to check "break" and "continue".
    // Return statements can be used everywhere...
    auto jump_token = node->JumpType();
    if (jump_token.type == TokenType::Return) return;

    // Generate exception if "break" or "continue" is used outside a loop.
    if (m_loop_nesting_level == 0)
    {
        auto message = std::format("jump statement \"{}\" cannot be used outside a loop",
            jump_token.lexeme
        );
        throw SemanticError(node->StartPos(), message);
    }
}

void JumpStmtUsageChecker::EnterLoop()
{
    ++m_loop_nesting_level;
}

void JumpStmtUsageChecker::ExitLoop()
{
    ++m_loop_nesting_level;
}

} // namespace mylang