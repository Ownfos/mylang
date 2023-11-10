#include "parser/routine/IParseRoutine.h"

namespace mylang
{

Token Accept(BufferedStream<Token>& token_stream, TokenType type)
{
    auto token = token_stream.GetNext();
    if (token.type != type)
    {
        // TODO: throw unexpected token exception
    }
    return token;
}

Token AcceptOneOf(BufferedStream<Token>& token_stream, const std::set<TokenType>& types)
{
    auto token = token_stream.GetNext();
    if (types.count(token.type) == 0)
    {
        // TODO: throw unexpected token exception
    }
    return token;
}

std::optional<Token> OptionalAccept(BufferedStream<Token>& token_stream, TokenType type)
{
    if (token_stream.Peek().type == type)
    {
        return token_stream.GetNext();
    }
    else
    {
        return {};
    }
}

std::optional<Token> OptionalAcceptOneOf(BufferedStream<Token>& token_stream, const std::set<TokenType>& types)
{
    if (types.count(token_stream.Peek().type) != 0)
    {
        return token_stream.GetNext();
    }
    else
    {
        return {};
    }
}

} // namespace mylang
