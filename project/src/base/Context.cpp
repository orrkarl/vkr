#include "Context.h"

#include "RootDevice.h"

namespace nr::base {

cl_context createFromType(const cl_context_properties* properties, Context::DeviceTypeBitField deviceType)
{
    Status status = CL_SUCCESS;

    auto ret = clCreateContextFromType(properties, deviceType, nullptr, nullptr, &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

cl_context createFromDeviceList(const cl_context_properties* properties, std::vector<RootDevice>& devices)
{
    Status status = CL_SUCCESS;

    if (devices.size() > std::numeric_limits<U32>::max()) {
        throw ContextCreateException(CL_INVALID_VALUE);
    }

    std::vector<cl_device_id> rawIDs(devices.size());
    std::transform(
        devices.cbegin(), devices.cend(), rawIDs.begin(), [](RootDevice& dev) { return dev.rawHandle(); });

    auto ret = clCreateContext(
        properties, static_cast<U32>(rawIDs.size()), rawIDs.data(), nullptr, nullptr, &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

Context::Context(const cl_context_properties* properties, std::vector<RootDevice>& devices)
    : m_context(createFromDeviceList(properties, devices))
{
}

Context::Context(const cl_context_properties* properties, DeviceTypeBitField deviceType)
    : m_context(createFromType(properties, deviceType))
{
}

cl_context Context::rawHandle() { return m_context; }

}