#pragma once

#include "../general/predefs.h"

#include "Wrapper.h"

namespace nr
{

class NR_SHARED Device : public Wrapper<cl_device_id>
{
// Functions and Constructors
public:
    static makeDefault(Device provided)
    {
        defaultDevice = provided;
    }

    static getDefault()
    {
        return defaultDevice;
    }

    Device()
        : Wrapped()
    {
    }

    explicit Device(const cl_device_id& Device, const NRbool retain = false)
        : Wrapped(Device, retain)
    {
    }

    Device(const Device& other)
        : Wrapped(other)
    {
    }

    Device(Device&& other)
        : Wrapped(other)
    {
    }

    Device& operator=(const Device& other)
    {
        return Wrapped::operator=(other);
    }

    Device& operator=(Device&& other)
    {
        return Wrapped::operator=(other);
    }

    operator cl_device_id() const 
    {
        return object;
    }

// Fields
private:
    static Device defaultDevice;
};

}