#include "parser/SemanticError.h"
#include <format>
#include <sstream>

namespace mylang
{

SemanticError::SemanticError(const SourcePos& location, std::string_view message)
    : m_location(location)
    , m_message(std::format("[Semantic Error][Ln {}, Col {}] {}",
        location.line,
        location.column,
        message
    ))
{}

const SourcePos& SemanticError::where() const
{
    return m_location;
}

const char* SemanticError::what() const
{
    return m_message.c_str();
}

} // namespace mylang
