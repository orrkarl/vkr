#include "Platform.h"

#include <algorithm>

namespace nr
{

std::vector<Platform> Platform::getAvailablePlatforms(cl_status& err)
{
    cl_uint platformCount;
    clGetPlatformIDs(0, nullptr, &platformCount);

    std::vector<cl_platform_id> platformIDs(platformCount);
    clGetPlatformIDs(platformCount, &platformIDs.front(), nullptr);

    std::vector<Platform> ret(platformCount);
    std::transform(platformIDs.cbegin(), platformIDs.cend(), ret.begin(), [](const cl_platform_id& plat){ return Platform(plat); });

    return ret;
}

Platform::Platform()
    : Wrapped()
{
}

Platform::Platform(const cl_platform_id& platform, const nr_bool retain)
    : Wrapped(platform, retain)
{
}

Platform::Platform(const Platform& other)
    : Wrapped(other)
{
}

Platform::Platform(Platform&& other)
    : Wrapped(other)
{
}

Platform& Platform::operator=(const Platform& other)
{
    Wrapped::operator=(other);
    return *this;
}

Platform& Platform::operator=(Platform&& other)
{
    Wrapped::operator=(other);
    return *this;
}

Platform::operator cl_platform_id() const 
{
    return object;
}

std::vector<Device> Platform::getDevicesByType(cl_device_type type, cl_status& err)
{
    cl_uint deviceCount;
    clGetDeviceIDs(object, type, 0, nullptr, &deviceCount);

    std::vector<cl_device_id> deviceIDs(deviceCount);
    clGetDeviceIDs(object, type, deviceCount, &deviceIDs.front(), nullptr);

    std::vector<Device> ret(deviceCount);
    std::transform(deviceIDs.cbegin(), deviceIDs.cend(), ret.begin(), [](const cl_device_id& dev){ return Device(dev); });

    return ret;
}

}