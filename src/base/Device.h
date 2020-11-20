#pragma once

#include <string>

#include "../predefs.h"

#include "Views.h"
#include "Wrapper.h"

namespace nr::base {

/**
 * @brief Represents an OpenCL physical root device, such as a specific GPU existing in the computer.
 *
 * A root device can only be created with a platform, and doesn't do lifetime management
 */
class RootDevice {
public:
    cl_device_id rawHandle();

    std::string name() const;

private:
    friend class Platform;

    explicit RootDevice(cl_device_id device);

    cl_device_id m_object;
};

}