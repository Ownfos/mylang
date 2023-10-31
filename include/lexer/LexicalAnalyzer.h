#ifndef MYLANG_LEXICAL_ANALYZER_H
#define MYLANG_LEXICAL_ANALYZER_H

#include "lexer/ILexicalAnalyzer.h"
#include "lexer/LookAheadManager.h"
#include <optional>

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

    // Keep reading characters until the end of a comment.
    // Returns false immediately if we are not at the beginning of a comment.
    bool TryRemoveComment();

    Token FindLongestMatch();

    std::optional<Token> TryFindSingleCharToken();
    std::optional<Token> TryFindAtMostTwoCharToken();
    std::optional<Token> TryFindNumericLiteral();
    std::optional<Token> TryFindStringLiteral();
    std::optional<Token> TryFindIdentifier();

    std::unique_ptr<ISourceFile> m_source_file;
    LookAheadManager m_lookahead;
};

} // namespace mylang


#endif // MYLANG_LEXICAL_ANALYZER_H
