#ifndef MYLANG_I_SOURCE_FILE_H
#define MYLANG_I_SOURCE_FILE_H

#include "common/IStream.h"
#include "file/SourceChar.h"

namespace mylang
{

// Represents a stream of characters for a source code.
// Every child class should implement three virtual functions:
// - bool IsFinished() const;
// - char CurrentChar() const;
// - void LoadNextChar();
class ISourceFile : public IStream<SourceChar>
{
public:
    // Implement IStream interface using CurrentChar() and ReadNext().
    virtual SourceChar GetNext() override;

protected:
    // Return the last scanned character (or '$' for EOF).
    // This function is guaranteed to be called
    // only after LoadNextChar() is executed at least once.
    virtual char CurrentChar() const = 0;

    // Fetch next character.
    // Initial LoadNext() should load the first letter.
    // This function is guaranteed to be called
    // only when IsFinished() is false.
    virtual void LoadNextChar() = 0;

private:
    // Set m_pos to the position of next character.
    // Note that we need to examine current character
    // to handle line feed in case of '\n'.
    void MovePosToNext();

    // This will keep track of current head position.
    SourcePos m_current_pos;

    // Used to set initial value of m_pos to {1, 1}
    // when GetNext() is called for the first time.
    bool m_first_char_not_loaded = true;
};

} // namespace mylang

#endif // MYLANG_I_SOURCE_FILE_H
