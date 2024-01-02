#ifndef MYLANG_OUTPUT_FILE_H
#define MYLANG_OUTPUT_FILE_H

#include "file/IOutputFile.h"
#include <fstream>

namespace mylang
{

class OutputFile : public IOutputFile
{
public:
    virtual void Open(const std::filesystem::path& path) override;
    virtual void Close() override;

    virtual void Print(std::string_view msg) override;

private:
    std::ofstream m_ostream;
};

} // namespace mylang

#endif // MYLANG_OUTPUT_FILE_H
