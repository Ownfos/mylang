#include "parser/routine/TypeParser.h"
#include "parser/type/base/DataType.h"
#include "parser/type/base/FuncType.h"

namespace mylang
{

TypeParser::TypeParser(std::shared_ptr<BufferedStream<Token>> token_stream)
    : IParseRoutine(token_stream)
{}

bool TypeParser::CanStartParsing()
{
    switch(Peek())
    {
    // Pameter usage (optional prefix)
    case TokenType::In:
    case TokenType::Out:
    case TokenType::InOut:

    // Primitive types
    case TokenType::IntType:
    case TokenType::FloatType:
    case TokenType::BoolType:
    case TokenType::StringType:

    // Struct type
    case TokenType::Identifier:

    // Function types
    case TokenType::LeftBracket:
        return true;

    default:
        return false;
    }
}

Type TypeParser::Parse()
{
    auto base_type = ParseBaseType();

    // ("[" int-literal "]")*
    auto array_sizes = std::vector<int>{};
    while (OptionalAccept(TokenType::LeftBracket))
    {
        auto size = Accept(TokenType::IntLiteral);
        array_sizes.push_back(std::stoi(size.lexeme));
        Accept(TokenType::RightBracket);
    }

    return Type(base_type, array_sizes);
}

// base-type       ::= data-type | func-type
// data-type       ::= "i32" | "f32" | "str" | "bool" | struct-type
// struct-type     ::= identifier
// func-type       ::= "[" "(" param-type-list? ")" ("->" type)? "]"
std::shared_ptr<IBaseType> TypeParser::ParseBaseType()
{
    // Case 1) primitive types and structs
    auto type = OptionalAcceptOneOf({
        TokenType::IntType,
        TokenType::FloatType,
        TokenType::BoolType,
        TokenType::StringType,
        TokenType::Identifier
    });
    if (type)
    {
        return std::make_shared<DataType>(type.value());
    }
    // Case 2) function type: "[(type 1, type 2, ..., type N) -> type]"
    else
    {
        Accept(TokenType::LeftBracket);

        // Paramter types: "(type 1, type 2, ..., type N)"
        Accept(TokenType::LeftParen);
        auto param_types = std::vector<ParamType>{};
        if (CanStartParsing())
        {
            // First param type comes immediately.
            param_types.push_back(ParseParamType());

            // Second to last param types come after comma.
            while (OptionalAccept(TokenType::Comma))
            {
                param_types.push_back(ParseParamType());
            }
        }
        Accept(TokenType::RightParen);

        // Return type: "-> type"
        auto return_type = std::optional<Type>{};
        if (OptionalAccept(TokenType::Arrow))
        {
            return_type = Parse();
        }

        Accept(TokenType::RightBracket);

        return std::make_shared<FuncType>(param_types, return_type);
    }
}

// param-type ::= param-usage? type
ParamType TypeParser::ParseParamType()
{
    // Default value for param-usage is "in".
    auto usage = TryParseParamUsage().value_or(ParamUsage::In);
    auto type = Parse();

    return ParamType{type, usage};
}

// param-usage ::= "in" | "out" | "inout"
std::optional<ParamUsage> TypeParser::TryParseParamUsage()
{
    if (OptionalAccept(TokenType::In))
    {
        return ParamUsage::In;
    }
    else if (OptionalAccept(TokenType::Out))
    {
        return ParamUsage::Out;
    }
    else if (OptionalAccept(TokenType::InOut))
    {
        return ParamUsage::InOut;
    }
    else
    {
        return {};
    }
}

} // namespace mylang
