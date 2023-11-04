#ifndef MYLANG_SOURCE_CHAR_H
#define MYLANG_SOURCE_CHAR_H

#include "file/SourcePos.h"
#include <compare>

namespace mylang
{

// Represents a character in a source file.
// Includes the ASCII code and its position.
struct SourceChar
{
    char ch;
    SourcePos pos;

    bool operator==(const SourceChar&) const = default;

    inline bool operator==(char ch) const
    {
        return this->ch == ch;
    };
};

} // namespace mylang

#endif // MYLANG_SOURCE_CHAR_H
