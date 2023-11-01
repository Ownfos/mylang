#ifndef MYLANG_TOKEN_H
#define MYLANG_TOKEN_H

#include "file/SourcePos.h"
#include <string>

namespace mylang
{

enum class TokenType
{
    Identifier,
    IntLiteral,
    FloatLiteral,
    StringLiteral,
    BoolLiteral,
    For,
    While,
    Break,
    Continue,
    If,
    Else,
    Return,
    Struct,
    Multiply, // *
    Divide, // /
    Plus, // +
    Minus, // -
    UnaryPlus, // ++
    UnaryMinus, // --
    MultiplyAssign, // *=
    DivideAssign, // /=
    PlusAssign, // +=
    MinusAssign, // -=
    Assign, // =
    Equal, // ==
    Not, // !
    NotEqual, // !=
    Less, // <
    LessEqual, // <=
    Greater, // >
    GreaterEqual, // >=
    And, // &&
    Or, // ||
    LeftParen, // (
    RightParen, // )
    LeftBrace, // {
    RightBrace, // }
    LeftBracket, // [
    RightBracket, // ]
    Comma, // ,
    Colon, // :
    Semicolon, // ;
    Period, // .
    Arrow, // ->
    EndOfFile,
    Error,
};

struct Token
{
    TokenType type;
    std::string lexeme;
    SourcePos start_pos;
    SourcePos end_pos;

    bool operator==(const Token&) const = default;
};

} // namespace mylang

#endif // MYLANG_TOKEN_H
