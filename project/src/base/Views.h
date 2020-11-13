#pragma once

#include "../predefs.h"

#include "Wrapper.h"

namespace nr::base {

using MemoryView = ObjectView<cl_mem>;
using DeviceView = ObjectView<cl_device_id>;

}