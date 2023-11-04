#ifndef MYLANG_LEXICAL_ANALYZER_H
#define MYLANG_LEXICAL_ANALYZER_H

#include "common/BufferedStream.h"
#include "file/ISourceFile.h"
#include "lexer/ILexicalAnalyzer.h"
#include <optional>

namespace mylang
{

// Split characters from source file and turn them into a stream of tokens.
class LexicalAnalyzer : public ILexicalAnalyzer
{
public:
    LexicalAnalyzer(std::unique_ptr<ISourceFile>&& source_file);

    virtual Token GetNextToken() override;

    bool IsFinished();

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

    // Remove comments.
    void RemoveSingleLineComment();
    void RemoveMultiLineComment();

    // Make a token instance out of all accepted characters (i.e., the lexeme buffer).
    Token CreateToken(TokenType type);

    Token FindLongestMatch();
    std::optional<Token> TryFindEOF();
    std::optional<Token> TryFindSingleCharToken();
    std::optional<Token> TryFindAtMostTwoCharToken();
    std::optional<Token> TryFindNumericLiteral();
    std::optional<Token> TryFindStringLiteral();
    std::optional<Token> TryFindIdentifier();

    BufferedStream<SourceChar> m_lookahead;
};

} // namespace mylang


#endif // MYLANG_LEXICAL_ANALYZER_H
