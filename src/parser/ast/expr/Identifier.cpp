#include "parser/ast/expr/Identifier.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

Identifier::Identifier(const Token& id)
    : m_id(id)
{}

void Identifier::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    visitor->PostorderVisit(this);
}

const SourcePos& Identifier::StartPos() const
{
    return m_id.start_pos;
}

std::string Identifier::ToString() const
{
    return m_id.lexeme;
}

} // namespace mylang
