#include "Platform.h"

#include "Exceptions.h"

#include <algorithm>

namespace nr::base {

std::vector<Platform> Platform::getAvailablePlatforms() {
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

    std::vector<Platform> ret;
    std::transform(platformIDs.cbegin(), platformIDs.cend(), std::back_inserter(ret),
                   [&](const cl_platform_id& plat) { return Platform(plat); });

    return ret;
}

Platform::Platform(cl_platform_id platform)
    : m_object(platform) {
}

std::vector<RootDevice> Platform::getDevicesByType(cl_device_type type) {
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

    std::vector<RootDevice> ret;
    std::transform(deviceIDs.cbegin(), deviceIDs.cend(), std::back_inserter(ret),
                   [](cl_device_id dev) { return RootDevice(dev); });

    return ret;
}

cl_platform_id Platform::rawHandle() {
    return m_object;
}

}