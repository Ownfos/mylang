#include "lexer/DummyLexicalAnalyzer.h"

namespace mylang
{

DummyLexicalAnalyzer::DummyLexicalAnalyzer(const std::vector<Token>& tokens)
    : m_tokens(tokens)
{}

Token DummyLexicalAnalyzer::GetNextToken()
{
    if (m_next_token_ind == m_tokens.size())
    {
        return Token{
            .type = TokenType::EndOfFile,
            .lexeme = '$'
        };
    }
    else
    {
        return m_tokens[m_next_token_ind++];
    }
}

} // namespace mylang
