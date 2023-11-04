#ifndef MYLANG_SOURCE_FILE_H
#define MYLANG_SOURCE_FILE_H

#include "file/ISourceFile.h"
#include <filesystem>
#include <fstream>

namespace mylang
{

// Reads an actual file as a character stream.
// The constructor will throw std::runtime_error if it fails to open a file.
class SourceFile : public ISourceFile
{
public:
    SourceFile(const std::filesystem::path& path);
    virtual ~SourceFile();

    virtual bool IsFinished() const override;
    virtual char CurrentChar() const override;
    virtual void LoadNextChar() override;

private:
    std::ifstream m_file;
    char m_current_char;
};

} // namespace mylang

#endif // MYLANG_SOURCE_FILE_H
