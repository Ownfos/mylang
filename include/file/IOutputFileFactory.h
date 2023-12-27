#ifndef MYLANG_I_OUTPUT_FILE_FACTORY_H
#define MYLANG_I_OUTPUT_FILE_FACTORY_H

#include "file/IOutputFile.h"
#include <memory>

namespace mylang
{

class IOutputFileFactory
{
public:
    virtual ~IOutputFileFactory() = default;

    virtual std::shared_ptr<IOutputFile> CreateOutputFile() const = 0;
};

} // namespace mylang


#endif // MYLANG_I_OUTPUT_FILE_FACTORY_H