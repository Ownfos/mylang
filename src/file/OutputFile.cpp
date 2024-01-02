#include "file/OutputFile.h"
#include <format>

namespace mylang
{

void OutputFile::Open(const std::filesystem::path& path)
{
    m_ostream.open(path);
    if (!m_ostream)
    {
        throw std::runtime_error(std::format("[I/O Error] failed to open output file on path '{}'", path));
    }
}

void OutputFile::Close()
{
    if (m_ostream.is_open())
    {
        m_ostream.close();
    }
}

void OutputFile::Print(std::string_view msg)
{
    m_ostream << msg;
}

} // namespace mylang
