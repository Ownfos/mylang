#ifndef MYLANG_I_SOURCE_FILE_H
#define MYLANG_I_SOURCE_FILE_H

#include "file/SourcePos.h"

namespace mylang
{
    
// TODO: make ISourceFile a generic stream so that lexer and parser can use it

// Represents a stream of characters for a source code.
// You can get the current character and its position while scanning one by one.
// Assumes that only ASCII characters are used.
// The first character of the source file (or '$' if the file was empty)
// should be available right after construction.
class ISourceFile
{
public:
    virtual ~ISourceFile() = default;

    // Returns most recently scanned character
    // and its position on the source code.
    // If we are at the end of file, '$' will be returned.
    virtual char CurrentChar() const = 0;
    virtual SourcePos CurrentPos() const = 0;

    // Returns true if we reached the end of file.
    virtual bool IsEOF() const = 0;

    // Reads a single character and update source position.
    // Does nothing if we already reached EOF.
    virtual void ReadNext() = 0;
};

} // namespace mylang

#endif // MYLANG_I_SOURCE_FILE_H
