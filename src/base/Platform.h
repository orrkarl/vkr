#pragma once

#include "../predefs.h"

#include <vector>

#include "Device.h"
#include "DeviceType.h"
#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(Platform, CLApiException);

/**
 * @brief Representation of a OpenCL platform - a collection of devices managed by a single vendor
 *
 * This is the entry point for OpenCL initialization and information (such as available extensions) query
 * @note platform objects don't do any lifetime management, so they're movable and copyable
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
     * @brief Acquires all available devices of a certain type
     *
     * @param type device type
     * @param[out] err internal OpenCL call status
     * @return std::vector<Device>
     */
    std::vector<Device> getDevicesByType(DeviceTypeBitField type);

private:
    explicit Platform(cl_platform_id platform);

    cl_platform_id m_object;
};

}