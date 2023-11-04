#include "lexer/LexicalAnalyzer.h"
#include "lexer/LexicalError.h"
#include <vector>
#include <tuple>
#include <cctype>
#include <format>

namespace mylang
{

LexicalAnalyzer::LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file)
    : m_lookahead(std::move(source_file))
{}

Token LexicalAnalyzer::GetNextToken()
{
    // Make sure we are at the start of a token.
    ProceedToTokenStart();

    // Find a token (which could be an EOF).
    return FindLongestMatch();
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
    while (!m_lookahead.IsFinished() && IsWhitespace(m_lookahead.Peek().ch))
    {
        is_something_removed = true;
        m_lookahead.Discard();
    }
    return is_something_removed;
}

bool LexicalAnalyzer::TryRemoveComment()
{
    // This might be a comment.
    if (m_lookahead.Peek() == '/')
    {
        m_lookahead.Accept();

        // Line comment detected!
        if (m_lookahead.Peek() == '/')
        {
            RemoveSingleLineComment();
            return true;
        }
        // Multi-line comment detected!
        else if (m_lookahead.Peek() == '*')
        {
            RemoveMultiLineComment();
            return true;
        }
        // It was just a division operator...
        else
        {
            // Undo reading '/'
            m_lookahead.Rewind();
            return false;
        }
    }
    // Comment not found.
    else
    {
        return false;
    }
}

void LexicalAnalyzer::RemoveSingleLineComment()
{
    // Remove the '/' in the lexeme buffer.
    m_lookahead.ClearAcceptHistory();

    // Discard everything until the end of current line or EOF.
    while(!m_lookahead.IsFinished() && m_lookahead.Peek().ch != '\n')
    {
        m_lookahead.Discard();
    }
}

void LexicalAnalyzer::RemoveMultiLineComment()
{
    // This line of code removes '/' of the comment start "/*"
    // and provide information about the source position.
    // The comment_start_pos will be used on exception message
    // if this turns out to be an unterminated comment.
    auto comment_start_pos = CreateToken(TokenType::Error).start_pos;

    // Discard the '*' of the comment start "/*"
    m_lookahead.Discard();

    // Discard everything until we meet "*/" or reach EOF.
    while (!m_lookahead.IsFinished())
    {
        // Might be the end of this comment
        if (m_lookahead.Peek() == '*')
        {
            m_lookahead.Discard();
            if (m_lookahead.Peek() == '/')
            {
                m_lookahead.Discard();
                return;
            }
        }
        else
        {
            m_lookahead.Discard();
        }
    }

    // Arriving here implies that we reached EOF without encoutering "*/"
    throw LexicalError(comment_start_pos, "unterminated multi-line comment");
}

Token LexicalAnalyzer::FindLongestMatch()
{
    if (auto token = TryFindEOF(); token.has_value())
    {
        return token.value();
    }
    else if (auto token = TryFindSingleCharToken(); token.has_value())
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
        return CreateToken(TokenType::Error);
    }
}

std::optional<Token> LexicalAnalyzer::TryFindEOF()
{
    // There are two reasons why Accept() is used ahead of checking IsFinished():
    // 1. IStream::Isfinished() returns true only after reading the end sentinel.
    //    This means that IsFinished() DOES NOT return turn true before we read the '$'.
    // 2. Even if we are already at the end before Accept(),
    //    we need to provide a lexeme for EOF token which is obtained with GetNext().
    m_lookahead.Accept();
    if (m_lookahead.IsFinished())
    {
        return CreateToken(TokenType::EndOfFile);
    }
    else
    {
        m_lookahead.Rewind();
        return {};
    }
}

std::optional<Token> LexicalAnalyzer::TryFindSingleCharToken()
{
    // List of all tokens that doesn't have any longer match.
    auto cases = std::vector<std::tuple<char, TokenType>>{
        {'(', TokenType::LeftParen},
        {')', TokenType::RightParen},
        {'{', TokenType::LeftBrace},
        {'}', TokenType::RightBrace},
        {'[', TokenType::LeftBracket},
        {']', TokenType::RightBracket},
        {',', TokenType::Comma},
        {':', TokenType::Colon},
        {';', TokenType::Semicolon},
        {'.', TokenType::Period},
    };
    for (const auto& [ch, type] : cases)
    {
        if (m_lookahead.Peek() == ch)
        {
            m_lookahead.Accept();
            return CreateToken(type);
        }
    }
    
    // Pattern mismatch.
    return {};
}

std::optional<Token> LexicalAnalyzer::TryFindAtMostTwoCharToken()
{
    // List of all tokens that require one more lookahead.
    // First type is for single character match,
    // while the second type is for complete match.
    auto cases = std::vector<std::tuple<std::string, TokenType, TokenType>>{
        {"*=", TokenType::Multiply, TokenType::MultiplyAssign},
        {"/=", TokenType::Divide, TokenType::DivideAssign},
        {"==", TokenType::Assign, TokenType::Equal},
        {"!=", TokenType::Not, TokenType::NotEqual},
        {"<=", TokenType::Less, TokenType::LessEqual},
        {">=", TokenType::Greater, TokenType::GreaterEqual},
        {"&&", TokenType::Error, TokenType::And},
        {"||", TokenType::Error, TokenType::Or},
    };
    for (const auto& [candidate, type1, type2] : cases)
    {
        if (m_lookahead.Peek() == candidate[0])
        {
            m_lookahead.Accept();

            // Completely matched.
            if (m_lookahead.Peek() == candidate[1])
            {
                m_lookahead.Accept();
                return CreateToken(type2);
            }
            // Only the first character matched.
            else
            {
                return CreateToken(type1);
            }
        }
    }

    // += ++ -- -> -= are the special cases where
    // two or more token types share same prefix.
    if (m_lookahead.Peek() == '+')
    {
        m_lookahead.Accept();

        // '++'
        if (m_lookahead.Peek() == '+')
        {
            m_lookahead.Accept();
            return CreateToken(TokenType::UnaryPlus);
        }
        // '+='
        else if (m_lookahead.Peek() == '=')
        {
            m_lookahead.Accept();
            return CreateToken(TokenType::PlusAssign);
        }
        // '+'
        else
        {
            return CreateToken(TokenType::Plus);
        }
    }
    else if (m_lookahead.Peek() == '-')
    {
        m_lookahead.Accept();

        // '->'
        if (m_lookahead.Peek() == '>')
        {
            m_lookahead.Accept();
            return CreateToken(TokenType::Arrow);
        }
        // '-='
        else if (m_lookahead.Peek() == '=')
        {
            m_lookahead.Accept();
            return CreateToken(TokenType::MinusAssign);
        }
        else if (m_lookahead.Peek() == '-')
        {
            m_lookahead.Accept();
            return CreateToken(TokenType::UnaryMinus);
        }
        // '-'
        else
        {
            return CreateToken(TokenType::Minus);
        }
    }

    // Pattern mismatch.
    return {};
}

std::optional<Token> LexicalAnalyzer::TryFindNumericLiteral()
{
    // Integer and float literals start with a digit.
    if (std::isdigit(m_lookahead.Peek().ch))
    {
        while (std::isdigit(m_lookahead.Peek().ch))
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
            if (std::isdigit(m_lookahead.Peek().ch))
            {
                // Read more digits.
                while (std::isdigit(m_lookahead.Peek().ch))
                {
                    m_lookahead.Accept();
                }

                return CreateToken(TokenType::FloatLiteral);
            }
            // Integer literal was the longest valid match!
            // Rewind state back to when we encounterd '.'
            else
            {
                m_lookahead.Rewind();
                return CreateToken(TokenType::IntLiteral);
            }
        }
        else
        {
            return CreateToken(TokenType::IntLiteral);
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
    if (m_lookahead.Peek() == '"')
    {
        m_lookahead.Accept();

        while (!m_lookahead.IsFinished() && m_lookahead.Peek() != '\n')
        {
            if (m_lookahead.Peek() == '\\')
            {
                m_lookahead.Accept();

                // Check if this is a valid escape sequence.
                auto valid_escape_sequences = std::string("nrt\\\"'");
                auto is_valid = valid_escape_sequences.find(m_lookahead.Peek().ch) != std::string::npos;
                if (is_valid)
                {
                    m_lookahead.Accept();
                }
                // This is an unexpected escape sequence.
                else
                {
                    m_lookahead.Accept();
                    auto token = CreateToken(TokenType::Error);
                    auto message = std::format("illegal escape sequence in string literal \"{}\"", token.lexeme);
                    throw LexicalError(token.end_pos, message);
                }
            }
            else if (m_lookahead.Peek() == '"')
            {
                m_lookahead.Accept();
                return CreateToken(TokenType::StringLiteral);
            }
            else
            {
                m_lookahead.Accept();
            }
        }

        // Arriving here implies that we reached EOF
        // or newline without encoutering closing '"'.
        auto token = CreateToken(TokenType::StringLiteral);
        auto message = std::format("unterminated string literal [{}]", token.lexeme);
        throw LexicalError(token.start_pos, message);
    }
    // Pattern mismatch.
    else
    {
        return {};
    }
}

// A utility function used in TryFindIdentifier().
// Since all keywords and bool literals have overlapping pattern with id,
// we simply match everything as an identifier and change its type iff it was a reserved word.
void ChangeTypeIfReservedWord(Token& token)
{
    // A database of (lexeme, type) tuple for all reserved words.
    auto special_cases = std::vector<std::tuple<std::string, TokenType>>{
        {"i32",      TokenType::IntType},
        {"f32",      TokenType::FloatType},
        {"bool",     TokenType::BoolType},
        {"str",      TokenType::StringType},
        {"true",     TokenType::BoolLiteral},
        {"false",    TokenType::BoolLiteral},
        {"for",      TokenType::For},
        {"while",    TokenType::While},
        {"break",    TokenType::Break},
        {"continue", TokenType::Continue},
        {"if",       TokenType::If},
        {"else",     TokenType::Else},
        {"return",   TokenType::Return},
        {"struct",   TokenType::Struct},
        {"func",     TokenType::Func},
        {"in",       TokenType::In},
        {"out",      TokenType::Out},
        {"inout",    TokenType::InOut},
        {"module",   TokenType::Module},
        {"import",   TokenType::Import},
        {"export",   TokenType::Export},
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
    char curr = m_lookahead.Peek().ch;

    // Identifiers can start with '_' or an alphabet.
    if (curr == '_' || std::isalpha(curr))
    {
        // Keep accepting we meet something other than alphabet, number, or '_'.
        do
        {
            m_lookahead.Accept();
            curr = m_lookahead.Peek().ch;
        }
        while (curr == '_' || std::isalpha(curr) || std::isdigit(curr));

        // Create token and check if it is a reserved word (keywords, bool literal).
        // If it wasn't a reserved word, keep its type as Idendifier.
        auto token = CreateToken(TokenType::Identifier);
        ChangeTypeIfReservedWord(token);

        return token;
    }
    // Pattern mismatch.
    else
    {
        return {};
    }
}

Token LexicalAnalyzer::CreateToken(TokenType type)
{
    auto lexeme_buffer = m_lookahead.GetAcceptHistory();

    // Accumulate lexeme string.
    auto lexeme = std::string("");
    for (auto [ch, pos] : lexeme_buffer)
    {
        lexeme.push_back(ch);
    }

    // Create token.
    auto token = Token{
        .type = type,
        .lexeme = lexeme,
        .start_pos = lexeme_buffer.front().pos,
        .end_pos = lexeme_buffer.back().pos,
    };

    // Discard used lexeme and reset.
    m_lookahead.ClearAcceptHistory();

    return token;
}

} // namespace mylang
