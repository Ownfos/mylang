#include "parser/ast/expr/Literal.h"
#include "parser/ast/visitor/IAbstractSyntaxTreeVisitor.h"
#include "parser/type/base/PrimitiveType.h"

namespace mylang
{

TokenType LiteralToTokenType(const Token& literal)
{
    switch(literal.type)
    {
    case TokenType::IntLiteral:
        return TokenType::IntType;
    case TokenType::FloatLiteral:
        return TokenType::FloatType;
    case TokenType::BoolLiteral:
        return TokenType::BoolType;
    case TokenType::StringLiteral:
        return TokenType::StringType;
    default:
        throw std::exception("non-literal token was used to create Literal node");
    }
}

Literal::Literal(const Token& literal)
    : m_literal(literal)
    , m_decl_type(CreatePrimiveType(LiteralToTokenType(literal)))
{}

void Literal::Accept(IAbstractSyntaxTreeVisitor* visitor)
{
    visitor->Visit(this);
}

const SourcePos& Literal::StartPos() const
{
    return m_literal.start_pos;
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
