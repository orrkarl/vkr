#include "RootDevice.h"
#include "Exceptions.h"

#include <memory>

namespace nr::base {

RootDevice::RootDevice(cl_device_id device)
    : m_object(device)
{
}

cl_device_id RootDevice::rawHandle() { return m_object; }

std::string RootDevice::name() const
{
    size_t len = 0;

    auto status = clGetDeviceInfo(m_object, CL_DEVICE_NAME, 0, nullptr, &len);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name size");
    }

    auto ret = std::make_unique<char[]>(len);

    status = clGetDeviceInfo(m_object, CL_DEVICE_NAME, len, ret.get(), nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name");
    }

    return std::string(ret.get(), len);
}

}