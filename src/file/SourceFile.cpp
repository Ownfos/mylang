#include "file/SourceFile.h"
#include <format>

namespace mylang
{

SourceFile::SourceFile(const std::filesystem::path& path)
    : m_file(path)
{
    if (m_file.fail())
    {
        throw std::runtime_error(std::format("failed to open file on path '{}'", path.string()));
    }

    // Load the first character.
    ReadNext();
}

SourceFile::~SourceFile()
{
    m_file.close();
}

char SourceFile::CurrentChar() const
{
    return m_current_char;
}

SourcePos SourceFile::CurrentPos() const
{
    return m_pos;
}

bool SourceFile::IsEOF() const
{
    return m_file.eof();
}

void SourceFile::ReadNext()
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

    // Read single character.
    // Note that std::ifstream::eof() returns true only after
    // we read -1, which implies the end of file, with get().
    m_current_char = m_file.get();
    if (m_current_char == static_cast<char>(-1)) {
        m_current_char = '$';
    }
}

} // namespace mylang
