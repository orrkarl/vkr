/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style wrapper for cl_platform_id
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <vector>

#include "Device.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Representation of a OpenCL platform - a collection of devices managed by a single vendor
 * 
 * This is the entry point for OpenCL initialziation and information (such as availabe extensions) query 
 */
class NRAPI Platform : public Wrapper<cl_platform_id>
{
public:
    /**
     * @brief Aquires all available OpenCL platforms
     * 
     * @param[out] err internal OpenCL call status
     * @return std::vector<Platform> all host available platforms
     */
    static std::vector<Platform> getAvailablePlatforms(cl_status& err);

    /**
     * @brief Construct a null platform
     * 
     */
    Platform();

    /**
     * @brief Allowes the C++ class to own a raw cl_platform_id
     * 
     * @param platform raw cl_platform_id object
     * @param retain unused in this version, as a cl_platform_id has no reference count
     */
    explicit Platform(const cl_platform_id& platform, const nr_bool retain = false);

    Platform(const Platform& other);

    Platform(Platform&& other);

    Platform& operator=(const Platform& other);

    Platform& operator=(Platform&& other);

    operator cl_platform_id() const;

    /**
     * @brief Aquires all available devices of a certain type
     * 
     * @param type device type
     * @param[out] err internal OpenCL call status
     * @return std::vector<Device> 
     */
    std::vector<Device> getDevicesByType(cl_device_type type, cl_status& err);
};

}