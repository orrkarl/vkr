#include "Device.h"
#include <memory>

namespace nr::base {

DeviceView::DeviceView(cl_device_id device)
    : m_rawObject(device) {
}

cl_device_id DeviceView::rawHandle() {
    return m_rawObject;
}

std::string Device::name() const {
    size_t len = 0;

    auto status = clGetDeviceInfo(m_object, CL_DEVICE_NAME, 0, nullptr, &len);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name size");
    }

    auto ret = std::make_unique<char[]>(len);

    status = clGetDeviceInfo(m_object, CL_DEVICE_NAME, len, ret.get(), nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query device name");
    }

    return std::string(ret.get(), len);
}

Device::Device(cl_device_id device)
    : m_object(device) {
}

DeviceView Device::view() {
    return DeviceView(m_object);
}

RootDevice::RootDevice(cl_device_id device)
    : Device(device) {
}

}