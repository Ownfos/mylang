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
}

const SourcePos& CompoundStmt::StartPos() const
{
    // TODO: decide which value to return...
    return {};
}

const std::vector<std::shared_ptr<Stmt>>& CompoundStmt::Statements() const
{
    return m_statements;
}

} // namespace mylang
