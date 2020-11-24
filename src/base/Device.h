#pragma once

#include <string>

#include "../predefs.h"

#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(Device, CLApiException);

class Device {
public:
    std::string name() const;

private:
    friend class CommandQueue;
    friend class Context;
    friend class Module;
    friend class Platform;

    struct DeviceTraits {
        using Type = cl_device_id;

        static constexpr auto release = clReleaseDevice;
        static constexpr auto retain = clRetainDevice;
    };

    explicit Device(cl_device_id device, Bool retain);

    cl_device_id rawHandle() const;

    UniqueWrapper<DeviceTraits> m_object;
};

}