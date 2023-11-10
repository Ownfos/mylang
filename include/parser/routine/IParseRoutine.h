#ifndef MYLANG_I_PARSE_ROUTINE_H
#define MYLANG_I_PARSE_ROUTINE_H

#include "common/BufferedStream.h"
#include "lexer/Token.h"
#include <memory>
#include <set>
#include <optional>

namespace mylang
{

// Interface for parse routines that convert
// specific token pattern into an object of type T.
// It is mostly used to generate AST nodes,
// but T doesn't have to be a tree node.
template<typename T>
class IParseRoutine
{
public:
    IParseRoutine(std::shared_ptr<BufferedStream<Token>> token_stream);
    virtual ~IParseRoutine() = default;

    virtual bool CanStartParsing() = 0;
    virtual T Parse() = 0;

protected:
    // Helper functions for reading tokens of specific type (or types).
    // They all accept current token when types match,
    // but have different behavior on mismatch.
    // - Accept, AcceptOneOf: throws exception
    // - OptionalAccept, OptionalAcceptOneOf: does nothing but return empty std::optional
    Token Accept(TokenType type);
    Token AcceptOneOf(const std::set<TokenType>& types);
    std::optional<Token> OptionalAccept(TokenType type);
    std::optional<Token> OptionalAcceptOneOf(const std::set<TokenType>& types);

    // Returns the lookahead token's type.
    TokenType Peek(int offset = 0);

    std::shared_ptr<BufferedStream<Token>> m_token_stream;
};

// Implementation file
#include "parser/routine/IParseRoutine.tpp"

} // namespace mylang

#endif // MYLANG_I_PARSE_ROUTINE_H
