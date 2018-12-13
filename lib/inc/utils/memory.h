#include <cstring>
#include <vector>

#include "../general/predefs.h"

#include "../rendering/DrawObject.h"

#pragma once

namespace nr
{

std::pair<NRuint, void*> unifyBuffers(const std::unordered_map<NRuint, Buffer*>& buffers)
{
    NRuint totalSize = 0;

    for (const auto& it : buffers)
    {
        totalSize += it.second->getSize();
    }

    totalSize = 0;

    // actually using malloc and voidptr in cpp, almost ashamed of myself...
    auto ret = malloc(totalSize);

    for (const auto& buffer : buffers)
    {
        std::memcpy(static_cast<void*>(static_cast<char*>(ret) + totalSize), buffer.second->getData(), buffer.second->getSize());
        totalSize += buffer.second->getSize();
    }

    return { totalSize, ret };
}

}
