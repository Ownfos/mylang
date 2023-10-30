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

    virtual char CurrentChar() const override;
    virtual SourcePos CurrentPos() const override;

    virtual bool IsEOF() const override;

    virtual void ReadNext() override;

private:
    std::ifstream m_file;
    char m_current_char = '$';

    // The column number will be incremented to 1
    // on the initial call of ReadNext() inside the constructor.
    SourcePos m_pos = {.line = 1, .column = 0};
};

} // namespace mylang

#endif // MYLANG_SOURCE_FILE_H
