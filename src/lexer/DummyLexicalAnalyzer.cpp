#include "lexer/DummyLexicalAnalyzer.h"

namespace mylang
{

DummyLexicalAnalyzer::DummyLexicalAnalyzer(const std::vector<Token>& tokens)
    : m_tokens(tokens)
{}

bool DummyLexicalAnalyzer::IsFinished() const
{
    return m_current_token_ind == m_tokens.size();
}

Token DummyLexicalAnalyzer::GetNext()
{
    // Move to the next position, unless it reaches out of bound.
    m_current_token_ind = std::min(m_current_token_ind + 1, static_cast<int>(m_tokens.size()));

    if (IsFinished())
    {
        return Token{
            .type = TokenType::EndOfFile,
            .lexeme = '$'
        };
    }
    else
    {
        return m_tokens[m_current_token_ind];
    }
}

} // namespace mylang
