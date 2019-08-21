#include "Device.h"

#include <memory>

namespace nr
{
Device::Device()
    : Wrapped()
{
}

Device::Device(const cl_device_id& device, const nr_bool retain)
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

string Device::name() const
{
	nr_size len;
	clGetDeviceInfo(object, CL_DEVICE_NAME, 0, nullptr, &len);

	auto ret = std::make_unique<nr_char[]>(len);

	clGetDeviceInfo(object, CL_DEVICE_NAME, len, ret.get(), nullptr);
	return string(ret.get(), len);
}

}