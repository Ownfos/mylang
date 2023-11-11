#include "parser/ast/expr/Literal.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"

namespace mylang
{

Literal::Literal(const Token& literal)
    : m_literal(literal)
{}

void Literal::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

std::string Literal::ToString() const
{
    return m_literal.lexeme;
}

} // namespace mylang
