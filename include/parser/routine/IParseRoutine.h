#ifndef MYLANG_I_PARSE_ROUTINE_H
#define MYLANG_I_PARSE_ROUTINE_H

#include "parser/ast/IAbstractSyntaxTree.h"
#include "common/BufferedStream.h"
#include "lexer/Token.h"
#include <memory>
#include <set>
#include <optional>

namespace mylang
{

Token Accept(BufferedStream<Token>& token_stream, TokenType type);
Token AcceptOneOf(const std::set<TokenType>& types);
std::optional<Token> OptionalAccept(TokenType type);
std::optional<Token> OptionalAcceptOneOf(const std::set<TokenType>& types);

class IParseRoutine
{
public:
    virtual ~IParseRoutine() = default;

    virtual std::shared_ptr<IAbstractSyntaxTree> Parse(BufferedStream<Token>& token_stream) const = 0;
};

} // namespace mylang

#endif // MYLANG_I_PARSE_ROUTINE_H
