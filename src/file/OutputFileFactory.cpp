#include "file/OutputFileFactory.h"
#include "file/OutputFile.h"

namespace mylang
{

std::shared_ptr<IOutputFile> OutputFileFactory::CreateOutputFile() const
{
    return std::make_shared<OutputFile>();
}

} // namespace mylang
