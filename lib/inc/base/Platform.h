#pragma once

#include "../general/predefs.h"

#include <vector>

#include "Device.h"
#include "Wrapper.h"

namespace nr
{

class NR_SHARED Platform : public Wrapper<cl_platform_id>
{
// Functions and Constructors
public:
    static makeDefault(Platform provided);

    static getDefault();

    static std::vector<Platform> getAvailablePlatforms(cl_status* err);

    Platform();

    explicit Platform(const cl_platform_id& Platform, const NRbool retain = false);

    Platform(const Platform& other);

    Platform(Platform&& other);

    Platform& operator=(const Platform& other);

    Platform& operator=(Platform&& other);

    operator cl_platform_id() const;

    std::vector<Device> getDevicesByType(cl_device_type type, cl_status* err);

// Fields
private:
    static Platform defaultPlatform;
};

}