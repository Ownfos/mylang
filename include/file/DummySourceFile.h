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

    virtual bool IsFinished() const override;
    virtual char CurrentChar() const override;
    virtual void LoadNextChar() override;

private:
    std::string m_content;
    int m_content_cursor = -1;
};

} // namespace mylang

#endif // MYLANG_DUMMY_SOURCE_FILE_H
