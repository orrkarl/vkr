#pragma once

#include "../general/predefs.h"

namespace nr
{

/**
 * Simple wrapper for cl::Buffer, may be removed in the next refactor as it isn't really neccessary
 * 
 * 
 **/
class NR_SHARED_EXPORT Buffer
{
public:
    Buffer(cl_mem_flags flags, const NRulong& size, cl_int* err = nullptr)
        : Buffer(flags, size, nullptr, err)
    {
    }

    Buffer()
    {
    }

    Buffer(cl_mem_flags flags, const NRulong& size, void* data = nullptr, cl_int* error = nullptr);

    cl::Buffer getBuffer() const { return m_buffer; }

    operator cl::Buffer() { return m_buffer; }

    cl_int resize(cl_mem_flags flags, const NRuint size);

private:
    cl::Buffer m_buffer;
};

#undef CTOR

}