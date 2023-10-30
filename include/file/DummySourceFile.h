#ifndef MYLANG_DUMMY_SOURCE_FILE_H
#define MYLANG_DUMMY_SOURCE_FILE_H

#include "file/ISourceFile.h"
#include <string>

namespace mylang
{

class DummySourceFile : public ISourceFile
{
public:
    DummySourceFile(std::string&& content);

    virtual char CurrentChar() const override;
    virtual SourcePos CurrentPos() const override;

    virtual bool IsEOF() const override;

    virtual void ReadNext() override;

private:
    std::string m_content;
    int m_content_cursor = 0;
    SourcePos m_pos = {1, 1};
};

} // namespace mylang

#endif // MYLANG_DUMMY_SOURCE_FILE_H
