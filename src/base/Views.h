#pragma once

#include "../predefs.h"

#include <string>
#include <vector>

#include "Wrapper.h"

namespace nr::base {

class MemoryView : public ObjectView<cl_mem> {
public:
    explicit MemoryView(cl_mem memory);

    size_t sizeOnDevice() const;
};

class DeviceView : public ObjectView<cl_device_id> {
public:
    explicit DeviceView(cl_device_id device);

    std::string name() const;
};

using KernelView = ObjectView<cl_kernel>;

}