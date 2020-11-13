#pragma once

#include "../predefs.h"
#include "BitField.h"
#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

class Context;

CL_TYPE_CREATE_EXCEPTION(BufferCreateException, "buffer")

/**
 * @brief Host representation of a plain OpenCL memory pointer
 *
 * This class represents the most simple memory scheme available. Think of this class as a "Device pointer".
 * Internally, this class wraps cl_mem and buffer clCreateBuffer
 * @note Host access (read/write) to the underlying data within the buffer is done with a CommandQueue
 *
 * @see CommandQueue
 * @see Context
 */
class Buffer {
public:
    enum class MemoryAccessFlag : cl_mem_flags {
        ReadWrite = CL_MEM_READ_WRITE,
        WriteOnly = CL_MEM_WRITE_ONLY,
        ReadOnly = CL_MEM_READ_ONLY,
        HostWriteOnly = CL_MEM_HOST_WRITE_ONLY,
        HostReadOnly = CL_MEM_HOST_READ_ONLY
    };

    enum class MemoryAllocateFlag : cl_mem_flags {
        None = 0,
        AllocateHostAccessible = CL_MEM_ALLOC_HOST_PTR,
        UseHostPtr = CL_MEM_USE_HOST_PTR,
        CopyHostPtr = CL_MEM_COPY_HOST_PTR
    };

    using MemoryAccessBitField = EnumBitField<MemoryAccessFlag>;

    Buffer(const Context& context, MemoryAccessBitField access, Bool hostAccessible, size_t size);

    Buffer(const Context& context, MemoryAccessBitField access, MemoryAllocateFlag allocate, size_t size,
        void* hostPtr);

    [[nodiscard]] size_t size() const;

private:
    struct BufferTraits {
        using Type = cl_mem;

        static constexpr auto release = clReleaseMemObject;
    };

    UniqueWrapper<BufferTraits> m_buffer;
};

}
