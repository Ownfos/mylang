#include "lexer/Token.h"

namespace mylang
{

// Is this really the best way...?
std::string TokenTypeName(TokenType type)
{
    switch(type)
    {
    case TokenType::Identifier:
        return "Identifier";
    case TokenType::IntLiteral:
        return "IntLiteral";
    case TokenType::FloatLiteral:
        return "FloatLiteral";
    case TokenType::StringLiteral:
        return "StringLiteral";
    case TokenType::BoolLiteral:
        return "BoolLiteral";
    case TokenType::IntType:
        return "i32";
    case TokenType::FloatType:
        return "f32";
    case TokenType::StringType:
        return "str";
    case TokenType::BoolType:
        return "bool";
    case TokenType::For:
        return "For";
    case TokenType::While:
        return "While";
    case TokenType::Break:
        return "Break";
    case TokenType::Continue:
        return "Continue";
    case TokenType::If:
        return "If";
    case TokenType::Else:
        return "Else";
    case TokenType::Return:
        return "Return";
    case TokenType::Struct:
        return "Struct";
    case TokenType::Func:
        return "Func";
    case TokenType::In:
        return "In";
    case TokenType::Out:
        return "Out";
    case TokenType::InOut:
        return "InOut";
    case TokenType::Module:
        return "Module";
    case TokenType::Import:
        return "Import";
    case TokenType::Multiply:
        return "Multiply";
    case TokenType::Divide:
        return "Divide";
    case TokenType::Plus:
        return "Plus";
    case TokenType::Minus:
        return "Minus";
    case TokenType::Increment:
        return "Increment";
    case TokenType::Decrement:
        return "Decrement";
    case TokenType::MultiplyAssign:
        return "MultiplyAssign";
    case TokenType::DivideAssign:
        return "DivideAssign";
    case TokenType::PlusAssign:
        return "PlusAssign";
    case TokenType::MinusAssign:
        return "MinusAssign";
    case TokenType::Assign:
        return "Assign";
    case TokenType::Equal:
        return "Equal";
    case TokenType::Not:
        return "Not";
    case TokenType::NotEqual:
        return "NotEqual";
    case TokenType::Less:
        return "Less";
    case TokenType::LessEqual:
        return "LessEqual";
    case TokenType::Greater:
        return "Greater";
    case TokenType::GreaterEqual:
        return "GreaterEqual";
    case TokenType::And:
        return "And";
    case TokenType::Or:
        return "Or";
    case TokenType::LeftParen:
        return "LeftParen";
    case TokenType::RightParen:
        return "RightParen";
    case TokenType::LeftBrace:
        return "LeftBrace";
    case TokenType::RightBrace:
        return "RightBrace";
    case TokenType::LeftBracket:
        return "LeftBracket";
    case TokenType::RightBracket:
        return "RightBracket";
    case TokenType::Comma:
        return "Comma";
    case TokenType::Colon:
        return "Colon";
    case TokenType::Semicolon:
        return "Semicolon";
    case TokenType::Period:
        return "Period";
    case TokenType::Arrow:
        return "Arrow";
    case TokenType::EndOfFile:
        return "EndOfFile";
    case TokenType::Error:
        return "Error";
    default:
        throw std::exception("Unexpected token type");
    }
}

} // namespace mylang
