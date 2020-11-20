#include "Views.h"

#include <algorithm>
#include <functional>
#include <limits>
#include <memory>

namespace nr::base {

MemoryView::MemoryView(cl_mem memory)
    : ObjectView(memory) {
}

size_t MemoryView::sizeOnDevice() const {
    size_t ret;

    auto status = clGetMemObjectInfo(m_rawObject, CL_MEM_SIZE, sizeof(ret), &ret, nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not acquire buffer size");
    }

    return ret;
}

}