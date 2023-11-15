#include "parser/SyntaxError.h"
#include <format>
#include <sstream>

namespace mylang
{

SyntaxError::SyntaxError(const SourcePos& location, std::string_view message)
    : m_location(location)
    , m_message(std::format("[Syntax Error][Ln {}, Col {}] {}",
        location.line,
        location.column,
        message
    ))
{}

const SourcePos& SyntaxError::where() const
{
    return m_location;
}

const char* SyntaxError::what() const
{
    return m_message.c_str();
}

UnexpectedTokenError::UnexpectedTokenError(
    const Token& token,
    const std::set<TokenType>& expected_types
)
    : m_token(token)
    , m_expected_types(expected_types)
{
    m_message = std::format("expected token with type {} but got {}",
        ExpectedTypeDescriptor(),
        TokenDescriptor()
    );
}

std::string UnexpectedTokenError::ExpectedTypeDescriptor() const
{
    if (m_expected_types.size() == 1)
    {
        return TokenTypeName(*m_expected_types.begin());
    }
    else
    {
        auto output = std::ostringstream{};

        auto begin = m_expected_types.begin();
        auto end = m_expected_types.end();
        auto last = --m_expected_types.end();

        auto it = begin;
        while (it != end)
        {
            if (it != begin)
            {
                output << ", ";
            }

            if (it == last)
            {
                output << "or ";
            }

            output << TokenTypeName(*it);
        }

        return output.str();
    }
}

std::string UnexpectedTokenError::TokenDescriptor() const
{
    return std::format("{{type: {}, lexeme: \"{}\"}}",
        TokenTypeName(m_token.type),
        m_token.lexeme
    );
}

const SourcePos& UnexpectedTokenError::Location() const
{
    return m_token.start_pos;
}

std::string_view UnexpectedTokenError::Description() const
{
    return m_message;
}

PatternMismatchError::PatternMismatchError(
    const ParseRoutineError& error,
    std::string_view pattern
)
    : m_location(error.Location())
    , m_message(std::format("failed to match pattern: {}\n-> {}",
        pattern,
        error.Description()
    ))
{}

const SourcePos& PatternMismatchError::Location() const
{
    return m_location;
}

std::string_view PatternMismatchError::Description() const
{
    return m_message;
}

LeftoverTokenError::LeftoverTokenError(const Token& token)
    : m_location(token.start_pos)
    , m_message(std::format("there were leftover tokens after parsing completed: \"{}\" ...", token.lexeme))
{}

const SourcePos& LeftoverTokenError::Location() const
{
    return m_location;
}

std::string_view LeftoverTokenError::Description() const
{
    return m_message;
}

} // namespace mylang
