#pragma once

#include "../general/predefs.h"

namespace nr
{

class Buffer
{
public:
    Buffer(cl_mem_flags flags, const NRulong& size, Error* err = nullptr)
        : Buffer(flags, size, nullptr, err)
    {
    }

    Buffer()
    {
    }

    Buffer(cl_mem_flags flags, const NRulong& size, void* data = nullptr, Error* error = nullptr);
    
    template<typename T>
    Buffer(cl_mem_flags flags, const NRulong& count, T* data, Error* err = nullptr)
        : Buffer(flags, count * sizeof(T), (void*) data, err)
    {
    }

    cl::Buffer getBuffer() const { return m_buffer; }

    operator cl::Buffer() { return m_buffer; }

    Error resize(cl_mem_flags flags, const NRuint size);

private:
    cl::Buffer m_buffer;
};

#undef CTOR

}