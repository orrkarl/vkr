#pragma once

#include "../predefs.h"

#include <vector>

#include "Device.h"
#include "DeviceType.h"
#include "Wrapper.h"

namespace nr::base {

/**
 * @brief Representation of a OpenCL platform - a collection of devices managed by a single vendor
 *
 * This is the entry point for OpenCL initialziation and information (such as availabe extensions) query
 */
class Platform {
public:
    /**
     * @brief Acquires all available OpenCL platforms
     *
     * @return std::vector<Platform> all host available platforms
     */
    static std::vector<Platform> getAvailablePlatforms();

    cl_platform_id rawHandle();

    /**
     * @brief Aquires all available devices of a certain type
     *
     * @param type device type
     * @param[out] err internal OpenCL call status
     * @return std::vector<Device>
     */
    std::vector<Device> getDevicesByType(cl_device_type type);
};

}