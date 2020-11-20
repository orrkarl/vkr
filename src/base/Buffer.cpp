#include "Buffer.h"

#include "Context.h"

namespace nr::base {

cl_mem create(cl_context context, Memory::AccessBitField access, Memory::AllocateFlag allocate, size_t size,
              void* hostPtr) {
    Status status = CL_SUCCESS;

    auto ret = clCreateBuffer(context,
                              static_cast<cl_mem_flags>(access) & static_cast<cl_mem_flags>(allocate), size,
                              hostPtr, &status);
    if (ret == nullptr) {
        throw BufferCreateException(status);
    }

    return ret;
}

Buffer::Buffer(Context& context, Memory::AccessBitField access, Bool hostAccessible, size_t size)
    : Memory(
        create(context.rawHandle(), access,
               hostAccessible ? Buffer::AllocateFlag::AllocateHostAccessible : Buffer::AllocateFlag::None,
               size, nullptr)) {
}

Buffer::Buffer(Context& context, Memory::AccessBitField access, Buffer::AllocateFlag allocate, size_t size,
               void* hostPtr)
    : Memory(create(context.rawHandle(), access, allocate, size, hostPtr)) {
}

}
