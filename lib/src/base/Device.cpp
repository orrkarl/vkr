#include <base/Device.h>

namespace nr
{

static Device::makeDefault(Device provided)
{
    defaultDevice = provided;
}

static Device::getDefault()
{
    return defaultDevice;
}

Device::Device()
    : Wrapped()
{
}

Device::Device(const cl_device_id& Device, const NRbool retain = false)
    : Wrapped(Device, retain)
{
}

Device::Device(const Device& other)
    : Wrapped(other)
{
}

Device::Device(Device&& other)
    : Wrapped(other)
{
}

Device& Device::operator=(const Device& other)
{
    return Wrapped::operator=(other);
}

Device& Device::operator=(Device&& other)
{
    return Wrapped::operator=(other);
}

Device::operator cl_device_id() const 
{
    return object;
}

}