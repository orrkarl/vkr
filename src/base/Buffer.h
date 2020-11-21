#pragma once

#include "Memory.h"

namespace nr::base {

class Context;

CL_TYPE_CREATE_EXCEPTION(Buffer, MemoryCreateException);

class Buffer : public Memory {
public:
    Buffer(Context& context, AccessBitField access, AllocateBitField allocate, size_t size, void* hostPtr);
};

}