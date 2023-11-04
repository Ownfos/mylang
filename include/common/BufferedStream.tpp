template<typename T>
BufferedStream<T>::BufferedStream(std::unique_ptr<IStream<T>>&& input_stream)
    : m_input_stream(std::move(input_stream))
{}

template<typename T>
bool BufferedStream<T>::IsFinished() const
{
    // Note: m_lookaheads always store at least one data
    // so that we can immediatly
    return m_lookaheads.size() == 0 && m_input_stream->IsFinished();
}

template<typename T>
T BufferedStream<T>::GetNext()
{
    // Read new data if there are no lookaheads.
    if (m_lookaheads.empty())
    {
        return m_input_stream->GetNext();
    }
    // Use the existing lookahead data.
    else
    {
        auto data = m_lookaheads.front();
        m_lookaheads.erase(m_lookaheads.begin());

        return data;
    }
}

template<typename T>
T BufferedStream<T>::Peek(unsigned int offset)
{
    // Read until we reach the specified offset.
    while (m_lookaheads.size() < offset + 1)
    {
        m_lookaheads.push_back(m_input_stream->GetNext());
    }

    return m_lookaheads[offset];
}


template<typename T>
void BufferedStream<T>::Accept()
{
    auto current = GetNext();
    m_accept_history.push_back(current);
}

template<typename T>
void BufferedStream<T>::Discard()
{
    GetNext();
}

template<typename T>
void BufferedStream<T>::Rewind()
{
    // Transfer every data accepted since rewind checkpoint
    // from m_accept_history to m_lookaheads for reuse.

    // Calculate the range of elements to transfer.
    auto rewind_begin = m_accept_history.begin() + m_rewind_checkpoint;
    auto rewind_end = m_accept_history.end();

    // Copy to destination
    m_lookaheads.insert(m_lookaheads.begin(), rewind_begin, rewind_end);

    // Remove source
    m_accept_history.erase(rewind_begin, rewind_end);
}

template<typename T>
void BufferedStream<T>::MarkRewindCheckpoint()
{
    // Everything next to the last element will be rewinded.
    // Note: this also works when m_accept_history is empty!
    m_rewind_checkpoint = m_accept_history.size();
}

template<typename T>
void BufferedStream<T>::ClearAcceptHistory()
{
    m_accept_history.clear();
    m_rewind_checkpoint = 0;
}

template<typename T>
const std::vector<T>& BufferedStream<T>::GetAcceptHistory()
{
    return m_accept_history;
}