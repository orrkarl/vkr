#include "Context.h"

#include "../util/Containers.h"
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

cl_context Context::createFromDeviceList(Platform& platform, std::vector<Device>& devices) {
    Status status = CL_SUCCESS;

    auto rawIDs = util::extractSizeLimited<Device, cl_device_id>(devices, std::mem_fn(&Device::rawHandle));

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

Context::Context(Platform& platform, std::vector<Device>& devices)
    : m_context(createFromDeviceList(platform, devices)) {
}

Context::Context(Platform& platform, DeviceTypeBitField deviceType)
    : m_context(createFromType(platform, deviceType)) {
}

cl_context Context::rawHandle() {
    return m_context;
}

}