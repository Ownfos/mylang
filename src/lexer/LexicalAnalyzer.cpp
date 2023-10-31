#include "lexer/LexicalAnalyzer.h"
#include <vector>
#include <cctype>

namespace mylang
{

LexicalAnalyzer::LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file)
    : m_lookahead(std::move(source_file))
{}

Token LexicalAnalyzer::GetNextToken()
{
    // Make sure we are at the start of a token.
    ProceedToTokenStart();

    if (m_lookahead.IsEOF())
    {
        m_lookahead.Accept();
        return m_lookahead.CreateToken(TokenType::EndOfFile);
    }
    else
    {
        return FindLongestMatch();
    }
}

void LexicalAnalyzer::ProceedToTokenStart()
{
    // Keep removing white spaces and comments
    // until we cannot remove anymore, which means that
    // we are looking at the starting character of a token candidate.
    bool is_something_removed = true;
    while (is_something_removed)
    {
        is_something_removed = TryRemoveWhitespaces() || TryRemoveComment();
    }
}

// A utility function used in TryRemoveWhitespaces().
bool IsWhitespace(char ch)
{
    // Return true iff ch is one of the following characters.
    std::string whitespaces = " \t\n\r";
    return whitespaces.find(ch) != std::string::npos;
}

bool LexicalAnalyzer::TryRemoveWhitespaces()
{
    bool is_something_removed = false;
    while (!m_lookahead.IsEOF() && IsWhitespace(m_lookahead.Peek()))
    {
        is_something_removed = true;
        m_lookahead.Discard();
    }
    return is_something_removed;
}

bool LexicalAnalyzer::TryRemoveComment()
{
    // TODO: implement comment discarding.
    return false;
}

Token LexicalAnalyzer::FindLongestMatch()
{
    if (auto token = TryFindSingleCharToken(); token.has_value())
    {
        return token.value();
    }
    else if (auto token = TryFindAtMostTwoCharToken(); token.has_value())
    {
        return token.value();
    }
    else if (auto token = TryFindNumericLiteral(); token.has_value())
    {
        return token.value();
    }
    else if (auto token = TryFindStringLiteral(); token.has_value())
    {
        return token.value();
    }
    else if (auto token = TryFindIdentifier(); token.has_value())
    {
        return token.value();
    }
    else
    {
        // Unexpected character!
        m_lookahead.Accept();
        return m_lookahead.CreateToken(TokenType::Error);
    }
}

std::optional<Token> LexicalAnalyzer::TryFindSingleCharToken()
{
    return {};
}

std::optional<Token> LexicalAnalyzer::TryFindAtMostTwoCharToken()
{
    return {};
}

std::optional<Token> LexicalAnalyzer::TryFindNumericLiteral()
{
    // Integer and float literals start with a digit.
    if (std::isdigit(m_lookahead.Peek()))
    {
        while (std::isdigit(m_lookahead.Peek()))
        {
            m_lookahead.Accept();
        }

        // This could be a float literal.
        // Save current state and move forward.
        m_lookahead.MarkRewindCheckpoint();
        if (m_lookahead.Peek() == '.')
        {
            m_lookahead.Accept();
            // Valid float literal should have digits after '.'
            if (std::isdigit(m_lookahead.Peek()))
            {
                // Read more digits.
                while (std::isdigit(m_lookahead.Peek()))
                {
                    m_lookahead.Accept();
                }

                return m_lookahead.CreateToken(TokenType::FloatLiteral);
            }
            // Integer literal was the longest valid match!
            // Rewind state back to when we encounterd '.'
            else
            {
                m_lookahead.RewindUntilCheckpoint();
                return m_lookahead.CreateToken(TokenType::IntLiteral);
            }
        }
        else
        {
            return m_lookahead.CreateToken(TokenType::IntLiteral);
        }
    }
    // Pattern mismatch.
    else
    {
        return {};
    }
}

std::optional<Token> LexicalAnalyzer::TryFindStringLiteral()
{
    return {};
}

// A utility function used in TryFindIdentifier().
// Since all keywords and bool literals have overlapping pattern with id,
// we simply match everything as an identifier and change its type iff it was a reserved word.
void ChangeTypeIfReservedWord(Token& token)
{
    // A database of (lexeme, type) tuple for all reserved words.
    auto special_cases = std::vector<std::tuple<std::string, TokenType>>{
        {"true",        TokenType::BoolLiteral},
        {"false",       TokenType::BoolLiteral},
        {"for",         TokenType::For},
        {"while",       TokenType::While},
        {"break",       TokenType::Break},
        {"continue",    TokenType::Continue},
        {"if",          TokenType::If},
        {"else",        TokenType::Else},
        {"return",      TokenType::Return},
        {"struct",      TokenType::Struct},
    };

    // Check if one of them matches the lexeme.
    for (const auto& [keyword, type] : special_cases)
    {
        if (token.lexeme == keyword)
        {
            token.type = type;
            return;
        }
    }
}

std::optional<Token> LexicalAnalyzer::TryFindIdentifier()
{
    char curr = m_lookahead.Peek();

    // Identifiers can start with '_' or an alphabet.
    if (curr == '_' || std::isalpha(curr))
    {
        // Keep accepting we meet something other than alphabet, number, or '_'.
        do
        {
            m_lookahead.Accept();
            curr = m_lookahead.Peek();
        }
        while (curr == '_' || std::isalpha(curr) || std::isdigit(curr));

        // Create token and check if it is a reserved word (keywords, bool literal).
        // If it wasn't a reserved word, keep its type as Idendifier.
        auto token = m_lookahead.CreateToken(TokenType::Identifier);
        ChangeTypeIfReservedWord(token);

        return token;
    }
    // Pattern mismatch.
    else
    {
        return {};
    }
}

} // namespace mylang
