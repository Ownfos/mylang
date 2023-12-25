#include "file/DummyOutputFile.h"

namespace mylang
{

void DummyOutputFile::Open(const std::filesystem::path& path)
{
    m_ostream.clear();
}

void DummyOutputFile::Close()
{
    // Nothing to close.
}

void DummyOutputFile::Print(std::string_view msg)
{
    m_ostream << msg;
}

std::string DummyOutputFile::Content() const
{
    return m_ostream.str();
}

} // namespace mylang
