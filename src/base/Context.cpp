#include "Context.h"

#include "Device.h"

namespace nr::base {

cl_context createFromType(const cl_context_properties* properties, DeviceTypeBitField deviceType) {
    Status status = CL_SUCCESS;

    auto ret = clCreateContextFromType(properties, deviceType, nullptr, nullptr, &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

cl_context Context::createFromDeviceList(const cl_context_properties* properties,
                                         std::vector<DeviceView>& devices) {
    Status status = CL_SUCCESS;

    if (devices.size() > std::numeric_limits<U32>::max()) {
        throw ContextCreateException(CL_INVALID_VALUE);
    }

    std::vector<cl_device_id> rawIDs(devices.size());
    std::transform(devices.begin(), devices.end(), rawIDs.begin(),
                   [](DeviceView dev) { return dev.rawHandle(); });

    auto ret = clCreateContext(properties, static_cast<U32>(rawIDs.size()), rawIDs.data(), nullptr, nullptr,
                               &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

Context::Context(const cl_context_properties* properties, std::vector<DeviceView>& devices)
    : m_context(createFromDeviceList(properties, devices)) {
}

Context::Context(const cl_context_properties* properties, DeviceTypeBitField deviceType)
    : m_context(createFromType(properties, deviceType)) {
}

cl_context Context::rawHandle() {
    return m_context;
}

}