#pragma once

#include <string>

#include "../predefs.h"

#include "Wrapper.h"

namespace nr::base {

class DeviceView {
private:
    friend class CommandQueue;
    friend class Context;
    friend class Device;
    friend class Module;

    explicit DeviceView(cl_device_id rawDevice);

    cl_device_id rawHandle();

    cl_device_id m_rawObject;
};

class Device {
public:
    std::string name() const;

    DeviceView view();

protected:
    explicit Device(cl_device_id rawDevice);

private:
    struct DeviceTraits {
        using Type = cl_device_id;

        static constexpr auto release = clReleaseDevice;
        static constexpr auto retain = clRetainDevice;
    };

    UniqueWrapper<DeviceTraits> m_object;
};

/**
 * @brief Represents an OpenCL physical root device, such as a specific GPU existing in the computer.
 *
 * A root device can only be created with a platform
 */
class RootDevice : public Device {
private:
    friend class Platform;

    explicit RootDevice(cl_device_id device);
};

}