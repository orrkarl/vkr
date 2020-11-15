#include "RootDevice.h"

namespace nr::base {

RootDevice::RootDevice(cl_device_id device)
    : m_object(device) {
}

cl_device_id RootDevice::rawHandle() {
    return m_object;
}

DeviceView RootDevice::view() {
    return DeviceView(m_object);
}

}