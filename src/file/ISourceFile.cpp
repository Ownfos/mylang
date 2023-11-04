#include "file/ISourceFile.h"

namespace mylang
{

SourceChar ISourceFile::GetNext()
{
    // Proceed to the next position if we have something more.
    if (!IsFinished())
    {
        MovePosToNext();
        LoadNextChar();
    }

    return SourceChar{
        .ch = CurrentChar(),
        .pos = m_current_pos
    };
}

void ISourceFile::MovePosToNext()
{
    // This was the first time GetNext() was called!
    if (m_first_char_not_loaded)
    {
        // Make sure we initialize position only once.
        m_first_char_not_loaded = false;

        // Initialize the position to (1, 1).
        m_current_pos.column = 1;
        m_current_pos.line = 1;
    }
    else
    {
        // Move to the next line's start.
        if (CurrentChar() == '\n')
        {
            m_current_pos.column = 1;
            m_current_pos.line++;
        }
        // Move right by one.
        else
        {
            m_current_pos.column++;
        }
    }
}


} // namespace mylang
