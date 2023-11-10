#include "parser/ast/stmt/CompoundStmt.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

CompoundStmt::CompoundStmt(const std::vector<std::shared_ptr<Stmt>>& statements)
    : m_statements(statements)
{}

void CompoundStmt::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
    visitor->IncreaseDepth();
    for (const auto& stmt : m_statements)
    {
        stmt->Accept(visitor);
    }
    visitor->DecreaseDepth();
}

} // namespace mylang
