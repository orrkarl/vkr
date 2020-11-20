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

}