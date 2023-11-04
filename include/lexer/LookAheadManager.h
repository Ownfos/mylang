#ifndef MYLANG_LOOK_AHEAD_H
#define MYLANG_LOOK_AHEAD_H

#include "file/ISourceFile.h"
#include "lexer/Token.h"
#include <memory>
#include <stack>
#include <vector>

namespace mylang
{

// TODO: make lookahead generic so that lexer and parser can use it
class LookAheadManager
{
public:
    LookAheadManager(std::unique_ptr<ISourceFile>&& source_file);

    bool IsEOF() const;

    // Returns the current lookahead character.
    char Peek() const;

    // Add current lookahead to lexeme buffer and read next one.
    // Does nothing on EOF.
    void Accept();

    // Ignore current lookahead and read next one.
    void Discard();

    // Record current state.
    // Calling RewindUntilCheckpoint() restores this recorded state.
    void MarkRewindCheckpoint();

    // Rewind state back to when MarkRewindCheckpoint() was called.
    // Unused lookaheads will be stored in a buffer so that
    // following Accept() or Discard() can reuse these 'rewinded' lookaheads.
    void RewindUntilCheckpoint();

    // Make a token out of current lexeme buffer and clear the buffer.
    Token CreateToken(TokenType type);

    // Empty buffer and reset rewind checkpoint.
    void CleanUpLexemeBuffer();

private:
    std::unique_ptr<ISourceFile> m_source_file;

    // Contains characters that a token consists of.
    std::vector<SourceChar> m_lexeme_buffer;

    // Contains characters that are not used yet.
    // TOS is the current lookahead.
    std::stack<SourceChar> m_lookahead_buffer;

    // The checkpoint for RewindUntilCheckpoint().
    // Any character at lexeme buffer starting from this index
    // will be stored back to m_lookahead_buffer for reuse.
    // Therefore, 0 implies that lexeme buffer will be emptied.
    int m_rewind_checkpoint = 0;
};

} // namespace mylang

#endif // MYLANG_LOOK_AHEAD_H
