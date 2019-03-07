#pragma once

#include <cstring>
#include <vector>

#include "../general/predefs.h"

namespace nr
{

class Buffer;

namespace utils
{

NR_SHARED_EXPORT std::pair<NRuint, void*> unifyBuffers(const std::vector<Buffer*>& buffers);

template <typename T>
NRuint vectorByteSize(const std::vector<T>& vec)
{
    return vec.size() * sizeof(T);
}

}

}