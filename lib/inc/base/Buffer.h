/**
 * @file Buffer.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping the most basic OpenCL device memory access scheme - buffers
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"
#include "Wrapper.h"
#include "Context.h"

namespace nr
{

/**
 * @brief Wrapper around an OpenCL buffer. Templated to allow type-safe read and write to and from the buffer.
 * @tparam T The buffer's element type (only has meaning on the host side, as device side type can be different)
 */
template<typename T>
class Buffer : public Wrapper<cl_mem>
{
public:    
    Buffer(const cl_mem_flags& flags, const nr_ulong& count, cl_status* err = nullptr)
        : Buffer(flags, count, nullptr, err)
    {
    }

    Buffer(const Context& context, const cl_mem_flags& flags, const nr_ulong& count, cl_status* err = nullptr)
        : Buffer(context, flags, count, nullptr, err)
    {
    }

    
    Buffer(const Context& context, const cl_mem_flags& flags, const nr_ulong& count, T* data = nullptr, cl_status* error = nullptr)
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

    /**
     * @brief Aquire the size (in bytes) of the buffer in device memory
     * @param[out] err internal OpenCL call error status
	 * @return the buffer size (in bytes)
     */
    nr_size getDeviceByteSize(cl_status* err = nullptr) const
    {
        nr_size ret = 0;
		auto status = clGetMemObjectInfo(object, CL_MEM_SIZE, sizeof(nr_size), &ret, nullptr);
        if (err) *err = status;
        return ret;
    }

	nr_size getElementCount(cl_status* err = nullptr) const
	{
		nr_size ret = 0;
		auto status = clGetMemObjectInfo(object, CL_MEM_SIZE, sizeof(nr_size), &ret, nullptr);
		if (err) *err = status;
		return ret / sizeof(T);
	}
};

}