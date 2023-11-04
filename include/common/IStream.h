#ifndef MYLANG_I_STREAM_H
#define MYLANG_I_STREAM_H

namespace mylang
{

// An interface for objects that provide sequence of data.
template<typename T>
class IStream
{
public:
    virtual ~IStream() = default;

    // Returns true iff there are nothing left to fetch.
    // For example, IsFinished() becomes false
    // after a file reads the EOF sentinel.
    virtual bool IsFinished() const = 0;

    // Fetch one data and return it.
    virtual T GetNext() = 0;
};

} // namespace mylang


#endif // MYLANG_I_STREAM_H
