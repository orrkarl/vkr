/**
 * @file Platform.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping cl_platform_id
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

#include <vector>

#include "Device.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Wrapper class for cl_platform_id, a collection of devices (usually by the same manufacturer)
 * 
 */
class NRAPI Platform : public Wrapper<cl_platform_id>
{
public:
    static std::vector<Platform> getAvailablePlatforms(cl_status* err);

    Platform();

    explicit Platform(const cl_platform_id& platform, const nr_bool retain = false);

    Platform(const Platform& other);

    Platform(Platform&& other);

    Platform& operator=(const Platform& other);

    Platform& operator=(Platform&& other);

    operator cl_platform_id() const;

    std::vector<Device> getDevicesByType(cl_device_type type, cl_status* err);
};

}