#include "file/DummyOutputFileFactory.h"
#include "file/DummyOutputFile.h"

namespace mylang
{

std::shared_ptr<IOutputFile> DummyOutputFileFactory::CreateOutputFile() const
{
    return std::make_shared<DummyOutputFile>();
}

} // namespace mylang
