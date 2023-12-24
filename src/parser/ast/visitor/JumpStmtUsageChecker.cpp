#include "parser/ast/visitor/JumpStmtUsageChecker.h"
#include "parser/ast/Module.h"
#include "parser/ast/globdecl/FuncDecl.h"
#include "parser/ast/stmt/IfStmt.h"
#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/stmt/JumpStmt.h"
#include "parser/SemanticError.h"
#include <format>

namespace mylang
{

void JumpStmtUsageChecker::Visit(Module* node)
{
    for (const auto& decl : node->Declarations())
    {
        decl->Accept(this);
    }
}

void JumpStmtUsageChecker::Visit(FuncDecl* node)
{
    node->Body()->Accept(this);
}

void JumpStmtUsageChecker::Visit(IfStmt* node)
{
    node->ThenBranch()->Accept(this);
    if (auto else_branch = node->ElseBranch())
    {
        else_branch->Accept(this);
    }
}

void JumpStmtUsageChecker::Visit(CompoundStmt* node)
{
    for (const auto& stmt : node->Statements())
    {
        stmt->Accept(this);
    }
}

void JumpStmtUsageChecker::Visit(JumpStmt* node)
{
    // We only need to check "break" and "continue".
    // Return statements can be used everywhere...
    auto jump_token = node->JumpType();
    if (jump_token.type != TokenType::Return)
    {
        // Generate exception if "break" or "continue" is used outside a loop.
        auto message = std::format("jump statement \"{}\" cannot be used outside a loop",
            jump_token.lexeme
        );
        throw SemanticError(node->StartPos(), message);
    }
}

} // namespace mylang