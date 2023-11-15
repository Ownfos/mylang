template<typename T>
IParseRoutine<T>::IParseRoutine(std::shared_ptr<BufferedStream<Token>> token_stream)
    : m_token_stream(token_stream)
{}

template<typename T>
Token IParseRoutine<T>::Accept(TokenType type)
{
    auto token = m_token_stream->GetNext();
    if (token.type != type)
    {
        throw UnexpectedTokenError(token, {type});
    }
    return token;
}

template<typename T>
Token IParseRoutine<T>::AcceptOneOf(const std::set<TokenType>& types)
{
    auto token = m_token_stream->GetNext();
    if (types.count(token.type) == 0)
    {
        throw UnexpectedTokenError(token, types);
    }
    return token;
}

template<typename T>
std::optional<Token> IParseRoutine<T>::OptionalAccept(TokenType type)
{
    if (m_token_stream->Peek().type == type)
    {
        return m_token_stream->GetNext();
    }
    else
    {
        return {};
    }
}

template<typename T>
std::optional<Token> IParseRoutine<T>::OptionalAcceptOneOf(const std::set<TokenType>& types)
{
    if (types.count(m_token_stream->Peek().type) != 0)
    {
        return m_token_stream->GetNext();
    }
    else
    {
        return {};
    }
}

template<typename T>
TokenType IParseRoutine<T>::Peek(int offset)
{
    return m_token_stream->Peek(offset).type;
}
