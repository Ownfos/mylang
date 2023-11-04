#ifndef MYLANG_SOURCE_POS_H
#define MYLANG_SOURCE_POS_H

#include <compare>

namespace mylang
{

// Specifies a location inside a source code.
// This will be used to provide information about where an error occured.
// Coordinates start from (1, 1).
struct SourcePos
{
    int line;
    int column;

    bool operator==(const SourcePos&) const = default;
};

} // namespace mylang

#endif // MYLANG_SOURCE_POS_H
