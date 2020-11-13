#include "Platform.h"
#include "Exceptions.h"

#include <algorithm>

namespace nr::base {

std::vector<Platform> Platform::getAvailablePlatforms()
{
    U32 platformCount = 0;
    auto status = clGetPlatformIDs(0, nullptr, &platformCount);
    if (status != CL_SUCCESS) {
        throw PlatformCreateException(status, "could not query total platform count");
    }

    std::vector<cl_platform_id> platformIDs(platformCount);
    status = clGetPlatformIDs(platformCount, &platformIDs.front(), nullptr);
    if (status != CL_SUCCESS) {
        throw PlatformCreateException(status, "could not enumerate platforms");
    }

    std::vector<Platform> ret(platformCount);
    std::transform(platformIDs.cbegin(), platformIDs.cend(), ret.begin(),
        [&](const cl_platform_id& plat) { return Platform(plat); });

    return ret;
}

Platform::Platform(cl_platform_id platform)
    : m_object(platform)
{
}

std::vector<Device> Platform::getDevicesByType(cl_device_type type)
{
    cl_uint deviceCount;
    auto status = clGetDeviceIDs(m_object, type, 0, nullptr, &deviceCount);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query total device count");
    }

    std::vector<cl_device_id> deviceIDs(deviceCount);
    status = clGetDeviceIDs(m_object, type, deviceCount, &deviceIDs.front(), nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not enumerate devices");
    }

    std::vector<Device> ret(deviceCount);
    for (auto i = 0u; i < deviceCount; ++i) {
        status = clRetainDevice(deviceIDs[i]);
        if (status != CL_SUCCESS) {
            throw CLApiException(status, "could not retain device ID");
        }

        ret[i] = Device(deviceIDs[i]);
    }

    return ret;
}

cl_platform_id Platform::rawHandle() { return m_object; }

}