#ifndef MYLANG_DUMMY_OUTPUT_FILE_FACTORY_H
#define MYLANG_DUMMY_OUTPUT_FILE_FACTORY_H

#include "file/IOutputFileFactory.h"

namespace mylang
{

class DummyOutputFileFactory : public IOutputFileFactory
{
public:
    virtual std::shared_ptr<IOutputFile> CreateOutputFile() const override;
};

} // namespace mylang


#endif // MYLANG_DUMMY_OUTPUT_FILE_FACTORY_H
