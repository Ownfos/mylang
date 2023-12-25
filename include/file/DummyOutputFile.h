#ifndef MYLANG_DUMMY_OUTPUT_FILE_H
#define MYLANG_DUMMY_OUTPUT_FILE_H

#include "file/IOutputFile.h"
#include <sstream>

namespace mylang
{

class DummyOutputFile : public IOutputFile
{
public:
    virtual void Open(const std::filesystem::path& path) override;
    virtual void Close() override;

    virtual void Print(std::string_view msg) override;

    std::string Content() const;

private:
    std::stringstream m_ostream;
};

} // namespace mylang

#endif // MYLANG_DUMMY_OUTPUT_FILE_H
