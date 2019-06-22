#pragma once

#include "../general/predefs.h"

#include "Wrapper.h"

namespace nr
{

class NR_SHARED Platform : public Wrapper<cl_platform_id>
{
// Functions and Constructors
public:
    static makeDefault(Platform provided)
    {
        defaultPlatform = provided;
    }

    static getDefault()
    {
        return defaultPlatform;
    }

    static std::vector<Platform> getAvailablePlatforms(cl_status* err);

    Platform()
        : Wrapped()
    {
    }

    explicit Platform(const cl_platform_id& Platform, const NRbool retain = false)
        : Wrapped(Platform, retain)
    {
    }

    Platform(const Platform& other)
        : Wrapped(other)
    {
    }

    Platform(Platform&& other)
        : Wrapped(other)
    {
    }

    Platform& operator=(const Platform& other)
    {
        return Wrapped::operator=(other);
    }

    Platform& operator=(Platform&& other)
    {
        return Wrapped::operator=(other);
    }

    operator cl_platform_id() const 
    {
        return object;
    }

// Fields
private:
    static Platform defaultPlatform;
};

}