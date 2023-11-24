#include "parser/ast/expr/Literal.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/type/base/PrimitiveType.h"

namespace mylang
{

Literal::Literal(const Token& literal)
    : m_literal(literal)
    , m_decl_type(std::make_shared<PrimitiveType>(literal))
{}

void Literal::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->PreorderVisit(this);
    visitor->PostorderVisit(this);
}

std::string Literal::ToString() const
{
    return m_literal.lexeme;
}

const Type& Literal::DeclType() const
{
    return m_decl_type;
}

} // namespace mylang
