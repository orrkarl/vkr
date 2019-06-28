#pragma once

#include "../general/predefs.h"
#include "Wrapper.h"
#include "Context.h"

namespace nr
{

/**
 * Simple wrapper for opencl buffers, may be removed in the next refactor as it isn't really neccessary
 * Also, this class isn't "NRAPI" as it is templated, hence everything is on the header
 **/
template<typename T>
class Buffer : public Wrapper<cl_mem>
{
public:    
    Buffer(const cl_mem_flags& flags, const nr_ulong& count, cl_int* err = nullptr)
        : Buffer(flags, count, nullptr, err)
    {
    }

    
    Buffer(const cl_mem_flags& flags, const nr_ulong& count, T* data = nullptr, cl_int* error = nullptr)
        : Wrapped(clCreateBuffer(Context::getDefault(), flags, count * sizeof(T), data, error))
    {
    }

    Buffer(const Context& context, const cl_mem_flags& flags, const nr_ulong& count, cl_int* err = nullptr)
        : Buffer(context, flags, count, nullptr, err)
    {
    }

    
    Buffer(const Context& context, const cl_mem_flags& flags, const nr_ulong& count, T* data = nullptr, cl_int* error = nullptr)
        : Wrapped(clCreateBuffer(context, flags, count * sizeof(T), data, error))
    {
    }

    Buffer()
        : Wrapped()
    {
    }

    explicit Buffer(const cl_mem& buffer, const nr_bool retain = false)
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
        Wrapped::operator=(other);
        return *this;
    }

    Buffer& operator=(Buffer&& other)
    {
        Wrapped::operator=(other);
        return *this;
    }

    operator cl_mem() const 
    {
        return object;
    }

    const cl_mem& get() const
    {
        return object;
    }

    nr_size getSize() const
    {
        nr_size ret;
        clGetMemObjectInfo(object, CL_MEM_SIZE, sizeof(nr_size), &ret, nullptr);
        return ret;
    }
};

}