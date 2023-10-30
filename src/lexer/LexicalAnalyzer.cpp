#include "lexer/LexicalAnalyzer.h"

namespace mylang
{

LexicalAnalyzer::LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file)
    : m_source_file(std::move(source_file))
{}

Token LexicalAnalyzer::GetNextToken()
{
    ProceedToTokenStart();
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

bool LexicalAnalyzer::TryRemoveWhitespaces()
{
    bool is_something_removed = false;
    while (!m_source_file->IsEOF() && IsWhitespace(m_source_file->CurrentChar()))
    {
        is_something_removed = true;
        m_source_file->ReadNext();
    }
    return is_something_removed;
}

bool LexicalAnalyzer::IsWhitespace(char ch) const
{
    // Return true iff ch is one of the following characters.
    std::string whitespaces = " \t\n\r";
    return whitespaces.find(ch) != std::string::npos;
}

bool LexicalAnalyzer::TryRemoveComment()
{
    // TODO: implement comment discarding.
    return false;
}

Token LexicalAnalyzer::FindLongestMatch()
{
    // TODO: implement RE matching.
    return Token{.type = TokenType::EndOfFile};
}

} // namespace mylang
