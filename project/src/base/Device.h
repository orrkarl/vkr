#pragma once

#include <string>

#include "../predefs.h"

#include "Wrapper.h"

namespace nr::base {

/**
 * @brief Represents an OpenCL physical device, such as a specific GPU exsiting in the computer.
 *
 * This class is mostly used for configuration and initialization
 * @note creating a device should be done with a Platform, and one shouldn't call the ctor's of Device
 * directly
 */
class Device {
public:
    [[nodiscard]] cl_device_id rawHandle();

    [[nodiscard]] std::string name() const;

private:
    friend class Platform;

    struct DeviceTraits {
        static constexpr auto release = clReleaseDevice;
        using Type = cl_device_id;
    };

    explicit Device(cl_device_id device);

    UniqueWrapper<DeviceTraits> m_object;
};

}