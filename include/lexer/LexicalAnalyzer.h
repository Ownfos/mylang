#ifndef MYLANG_LEXICAL_ANALYZER_H
#define MYLANG_LEXICAL_ANALYZER_H

#include "lexer/ILexicalAnalyzer.h"
#include "file/ISourceFile.h"
#include <memory>
#include <stack>

namespace mylang
{

class LexicalAnalyzer : public ILexicalAnalyzer
{
public:
    LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file);

    virtual Token GetNextToken() override;

private:
    // Keep reading characters until we meet
    // non-whitespace, non-comment-starting character.
    void ProceedToTokenStart();

    // Keep reading characters until non-whitespace character appears.
    // Returns false if current character is not a whitespace character.
    bool TryRemoveWhitespaces();
    bool IsWhitespace(char ch) const;

    // Keep reading characters until the end of a comment.
    // Returns false immediately if we are not at the beginning of a comment.
    bool TryRemoveComment();

    Token FindLongestMatch();

    std::unique_ptr<ISourceFile> m_source_file;
    std::string m_lexeme_buffer;
    std::stack<char> m_lookahead_buffer;
};

} // namespace mylang


#endif // MYLANG_LEXICAL_ANALYZER_H
