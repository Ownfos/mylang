#ifndef MYLANG_BUFFERED_STREAM_H
#define MYLANG_BUFFERED_STREAM_H

#include "common/IStream.h"
#include <memory>
#include <vector>
#include <queue>

namespace mylang
{

// A decorator class for IStream<T> that provides extra features:
// - Peek() : allows lookahead without actually consuming data
// - Accept() : stores current data to a buffer and call GetNext()
// - Discard() : basically same as GetNext(), but doesn't return anything (i.e. ignore current data)
// - Rewind() : restore stream's state to a checkpoint specified with MarkRewindCheckpoint()
template<typename T>
class BufferedStream : public IStream<T>
{
public:
    BufferedStream(std::unique_ptr<IStream<T>>&& input_stream);

    // Provide the same interface of IStream<T>
    virtual bool IsFinished() const override;
    virtual T GetNext() override;

    // Returns lookahead data without consuming it.
    // Giving offset of 0 results in the same return value as GetNext().
    T Peek(unsigned int offset = 0);

    // Store current data to a buffer and load the next one.
    // Accept() invocations can be reverted using Rewind().
    void Accept();

    // Ignore current data and load the next one.
    // Basically identical to GetNext(), but the name 'discard'
    // better describes the intent of explicitly ignoring current one.
    void Discard();

    // Revert all Accept() calls to restore previous stream state.
    // This will make previous data reappear as if we never read them (thus 'rewind').
    void Rewind();

    // Save the current accept history and use it as checkpoint.
    // Next Rewind() call will revert stream state back to this point.
    void MarkRewindCheckpoint();

    // Clear buffer and rewind checkpoint.
    void ClearAcceptHistory();

    // Return list of all data recoreded by Accept().
    const std::vector<T>& GetAcceptHistory();

private:
    std::unique_ptr<IStream<T>> m_input_stream;
    std::vector<T> m_lookaheads;
    std::vector<T> m_accept_history;
    size_t m_rewind_checkpoint = 0;
};

// Implementation file
#include "common/BufferedStream.tpp"

} // namespace mylang


#endif // MYLANG_BUFFERED_STREAM_H
