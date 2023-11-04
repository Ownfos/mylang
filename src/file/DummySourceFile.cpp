#include "file/DummySourceFile.h"

namespace mylang
{

DummySourceFile::DummySourceFile(std::string&& content)
    : m_content(std::move(content))
{}

bool DummySourceFile::IsFinished() const
{
    return m_content_cursor == m_content.size();
}

char DummySourceFile::CurrentChar() const
{
    if (IsFinished())
    {
        return '$';
    }
    else
    {
        return m_content[m_content_cursor];
    }
}

void DummySourceFile::LoadNextChar()
{
    m_content_cursor++;
}

} // namespace mylang
