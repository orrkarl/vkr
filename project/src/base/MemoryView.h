#pragma once

#include "../predefs.h"

namespace nr::base {

class MemoryView {
public:
    explicit MemoryView(cl_mem memoryObj);

    cl_mem rawHandle();

private:
    cl_mem m_memory;
};

}