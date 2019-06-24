#include <base/Device.h>

namespace nr
{

void Device::makeDefault(const Device& provided)
{
    defaultDevice = provided;
}

Device Device::getDefault()
{
    return defaultDevice;
}

Device::Device()
    : Wrapped()
{
}

Device::Device(const cl_device_id& device, const NRbool retain)
    : Wrapped(device, retain)
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
    Wrapped::operator=(other);
    return *this;
}

Device& Device::operator=(Device&& other)
{
    Wrapped::operator=(other);
    return *this;
}

Device::operator cl_device_id() const 
{
    return object;
}

const cl_device_id& Device::get() const
{
    return object;
}

Device Device::defaultDevice = Device();

}