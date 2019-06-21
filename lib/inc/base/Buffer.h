#pragma once

#include "../general/predefs.h"
#include "Wrapper.h"
#include "Context.h"

namespace nr
{

/**
 * Simple wrapper for opencl buffers, may be removed in the next refactor as it isn't really neccessary
 * 
 **/
template<typename T>
class NR_SHARED Buffer : Wrapper<cl_mem>
{
public:    
    Buffer(const Context& context, const cl_mem_flags& flags, const NRulong& count, cl_int* err = nullptr)
        : Buffer(context, flags, count, nullptr, err)
    {
    }

    
    Buffer(const Context& context, const cl_mem_flags& flags, const NRulong& count, T* data = nullptr, cl_int* error = nullptr)
        : Wrapped(clCreateBuffer(context, flags, count * sizeof(T), data, error))
    {
    }

    Buffer()
        : Wrapped()
    {
    }

    explicit Buffer(const cl_mem& buffer, const NRbool retain)
        : Wrapped(buffer, retain)
    {
    }

    Buffer(const Buffer& other)
        : Wrapped(other)
    {
    }

    Buffer(Buffer&& other)
        : Wrapped(other)
    {
    }

    Buffer& operator=(const Buffer& other)
    {
        return Wrapped::operator=(other);
    }

    Buffer& operator=(Buffer&& other)
    {
        return Wrapped::operator=(other);
    }

    operator cl_mem() const 
    {
        return get();
    }
};

}