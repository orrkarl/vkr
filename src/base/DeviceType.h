#pragma once

#include "../predefs.h"

#include "BitField.h"

namespace nr::base {

enum class DeviceTypeFlag : cl_device_type {
    GPU = CL_DEVICE_TYPE_GPU,
    CPU = CL_DEVICE_TYPE_CPU,
    Accelerator = CL_DEVICE_TYPE_ACCELERATOR,
    Default = CL_DEVICE_TYPE_DEFAULT,
    Custom = CL_DEVICE_TYPE_DEFAULT,
    ALL = CL_DEVICE_TYPE_ALL
};

using DeviceTypeBitField = EnumBitField<DeviceTypeFlag>;

}
