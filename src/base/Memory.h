#pragma once

#include "../predefs.h"

#include "BitField.h"
#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

class Context;

CL_TYPE_CREATE_EXCEPTION(Memory, CLApiException);

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
class Memory {
public:
    enum class AccessFlag : cl_mem_flags {
        ReadWrite = CL_MEM_READ_WRITE,
        WriteOnly = CL_MEM_WRITE_ONLY,
        ReadOnly = CL_MEM_READ_ONLY,
        HostWriteOnly = CL_MEM_HOST_WRITE_ONLY,
        HostReadOnly = CL_MEM_HOST_READ_ONLY
    };

    enum class AllocateFlag : cl_mem_flags {
        None = 0,
        AllocateHostAccessible = CL_MEM_ALLOC_HOST_PTR,
        UseHostPtr = CL_MEM_USE_HOST_PTR,
        CopyHostPtr = CL_MEM_COPY_HOST_PTR
    };

    using AccessBitField = BitField<AccessFlag>;
    using AllocateBitField = BitField<AllocateFlag>;

    size_t sizeOnDevice() const;

protected:
    Memory(cl_mem memory);

private:
    friend class CommandQueue;
    friend class Kernel;

    struct MemoryTraits {
        using Type = cl_mem;

        static constexpr auto release = clReleaseMemObject;
        static constexpr auto retain = clRetainMemObject;
    };

    cl_mem rawHandle() const;

    UniqueWrapper<MemoryTraits> m_memory;
};

CREATE_ENUM_FLAG_OPS(Memory::AccessBitField)
CREATE_ENUM_FLAG_OPS(Memory::AllocateBitField)

}