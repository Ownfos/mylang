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

Type CreateTypeFromLiteral(const Token& literal)
{
    // Create type token from literal token.
    // example) "1234" => "i32"
    auto token_type = LiteralToTokenType(literal);
    auto token = Token{
        .type = token_type,
        .lexeme = TokenTypeName(token_type)
    };
    return Type(std::make_shared<PrimitiveType>(token));
}

Literal::Literal(const Token& literal)
    : m_literal(literal)
    , m_decl_type(CreateTypeFromLiteral(literal))
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
