#include "Buffer.h"

namespace nr::base {

BufferCreateException::BufferCreateException(Status errorCode)
    : CLApiException(errorCode, "could not create buffer")
{
}

Buffer::BufferTraits::Type Buffer::BufferTraits::create(cl_context context, Buffer::MemoryAccessBitField access, Buffer::MemoryAllocateFlag allocate, size_t size, void* hostPtr)
{
    Status status = CL_SUCCESS;

    auto ret = clCreateBuffer(context, static_cast<cl_mem_flags>(access) & static_cast<cl_mem_flags>(allocate), size, hostPtr, &status);
    if (ret == nullptr) {
        throw BufferCreateException(status);
    }

    return ret;
}

size_t Buffer::size() const
{
    size_t ret;

    auto status = clGetMemObjectInfo(m_buffer, CL_MEM_SIZE, sizeof(ret), &ret, nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not acquire buffer size");
    }

    return ret;
}

Buffer::Buffer(cl_context context, Buffer::MemoryAccessBitField access, Bool hostAccessible, size_t size)
    : m_buffer(context, access, hostAccessible ? Buffer::MemoryAllocateFlag::AllocateHostAccessible : Buffer::MemoryAllocateFlag::None, size, nullptr)
{
}

Buffer::Buffer(cl_context context, Buffer::MemoryAccessBitField access, Buffer::MemoryAllocateFlag allocate, size_t size, void* hostPtr)
    : m_buffer(context, access, allocate, size, hostPtr)
{
}

}