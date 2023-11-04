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

    // Returns true if we already reached the end.
    //
    // --- Important Note ---
    // We never know if we the stream is finished
    // until we actually try to read the next one on the end!
    // For example, IsFinished() becomes false
    // after a file reads the EOF sentinel.
    virtual bool IsFinished() const = 0;

    // Fetch one data and return it.
    virtual T GetNext() = 0;
};

} // namespace mylang


#endif // MYLANG_I_STREAM_H
