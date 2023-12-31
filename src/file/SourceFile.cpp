#include "file/SourceFile.h"
#include <format>

namespace mylang
{

SourceFile::SourceFile(const std::filesystem::path& path)
    : m_file(path)
{
    if (!m_file)
    {
        throw std::runtime_error(std::format("[I/O Error] failed to open input file on path '{}'", path.string()));
    }
}

SourceFile::~SourceFile()
{
    m_file.close();
}

bool SourceFile::IsFinished() const
{
    return m_file.eof();
}

char SourceFile::CurrentChar() const
{
    return m_current_char;
}

void SourceFile::LoadNextChar()
{
    // Read single character.
    // Note: get() returns -1 when we reach EOF.
    m_current_char = m_file.get();
    if (m_current_char == static_cast<char>(-1)) {
        m_current_char = '$';
    }
}

} // namespace mylang
