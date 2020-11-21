#include "Context.h"

#include "Device.h"

namespace nr::base {

cl_context createFromType(Platform& platform, DeviceTypeBitField deviceType) {
    Status status = CL_SUCCESS;

    cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM,
                                            reinterpret_cast<cl_context_properties>(platform.rawHandle()),
                                            0 };
    auto ret = clCreateContextFromType(properties, static_cast<cl_device_type>(deviceType), nullptr, nullptr,
                                       &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

cl_context Context::createFromDeviceList(Platform& platform, std::vector<DeviceView>& devices) {
    Status status = CL_SUCCESS;

    if (devices.size() > std::numeric_limits<U32>::max()) {
        throw ContextCreateException(CL_INVALID_VALUE);
    }

    std::vector<cl_device_id> rawIDs(devices.size());
    std::transform(devices.begin(), devices.end(), rawIDs.begin(),
                   [](DeviceView dev) { return dev.rawHandle(); });

    cl_context_properties properties[3] = { CL_CONTEXT_PLATFORM,
                                            reinterpret_cast<cl_context_properties>(platform.rawHandle()),
                                            0 };
    auto ret = clCreateContext(properties, static_cast<U32>(rawIDs.size()), rawIDs.data(), nullptr, nullptr,
                               &status);
    if (ret == nullptr) {
        throw ContextCreateException(status);
    }

    return ret;
}

Context::Context(Platform& platform, std::vector<DeviceView>& devices)
    : m_context(createFromDeviceList(platform, devices)) {
}

Context::Context(Platform& platform, DeviceTypeBitField deviceType)
    : m_context(createFromType(platform, deviceType)) {
}

cl_context Context::rawHandle() {
    return m_context;
}

}