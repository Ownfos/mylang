#include "lexer/LexicalError.h"
#include <format>

namespace mylang
{
    
LexicalError::LexicalError(const SourcePos& location, const std::string& message)
    : m_location(location)
    , m_message(std::format("[Lexical Error][Ln {}, Col {}] {}",
        location.line,
        location.column,
        message
    ))
{}

const SourcePos& LexicalError::where() const
{
    return m_location;
}

const char* LexicalError::what() const
{
    return m_message.c_str();
}

} // namespace mylang
