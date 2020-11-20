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

DeviceView::DeviceView(cl_device_id device)
    : ObjectView(device) {
}

std::string DeviceView::name() const {
    size_t len = 0;

    auto status = clGetDeviceInfo(m_rawObject, CL_DEVICE_NAME, 0, nullptr, &len);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name size");
    }

    auto ret = std::make_unique<char[]>(len);

    status = clGetDeviceInfo(m_rawObject, CL_DEVICE_NAME, len, ret.get(), nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name");
    }

    return std::string(ret.get(), len);
}

}