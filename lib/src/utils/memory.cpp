#include <general/predefs.h>
#include <buffers/Buffer.h>

#include <utils/memory.h>


namespace nr
{

namespace utils
{

std::pair<NRuint, void*> unifyBuffers(const std::vector<Buffer*>& buffers)
{
    NRuint totalSize = 0;

    for (const auto& it : buffers)
    {
        totalSize += it->getByteSize();
    }

    auto ret = malloc(totalSize);

    totalSize = 0;

    // actually using malloc and voidptr in cpp, almost ashamed of myself...

    for (const auto& buffer : buffers)
    {
        memcpy((char*)ret + totalSize, buffer->getData(), buffer->getByteSize());
        totalSize += buffer->getByteSize();
    }

    return { totalSize, ret };
}

}

}
