#include "Memory.h"

#include "Context.h"

namespace nr::base {

size_t Memory::sizeOnDevice() const {
    size_t ret;

    auto status = clGetMemObjectInfo(m_memory, CL_MEM_SIZE, sizeof(ret), &ret, nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not acquire buffer size");
    }

    return ret;
}

Memory::Memory(cl_mem memory)
    : m_memory(memory) {
}

cl_mem Memory::rawHandle() const {
    return m_memory;
}

}