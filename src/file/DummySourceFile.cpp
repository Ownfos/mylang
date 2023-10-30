#include "file/DummySourceFile.h"

namespace mylang
{

DummySourceFile::DummySourceFile(std::string&& content)
    : m_content(std::move(content))
{}

char DummySourceFile::CurrentChar() const
{
    if (IsEOF())
    {
        return '$';
    }
    else
    {
        return m_content[m_content_cursor];
    }
}

SourcePos DummySourceFile::CurrentPos() const
{
    return m_pos;
}

bool DummySourceFile::IsEOF() const
{
    return m_content_cursor == m_content.size();
}

void DummySourceFile::ReadNext()
{
    // Do nothing if we are at EOF.
    if (IsEOF()) return;

    // Calculate next source position.
    if (CurrentChar() == '\n')
    {
        m_pos.column = 1;
        m_pos.line++;
    }
    else
    {
        m_pos.column++;
    }

    // Move cursor position by one.
    m_content_cursor++;
}

} // namespace mylang
