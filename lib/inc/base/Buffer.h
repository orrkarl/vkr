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
class NRAPI Buffer : public Wrapper<cl_mem>
{
public:
	template <typename T>
	static Buffer make(const Context& context, const cl_mem_flags& flags, const nr_uint& count, T* data, cl_status* status = nullptr)
	{
		return Buffer(context, flags, static_cast<nr_size>(count * sizeof(T)), data, status);
	}

	template <typename T>
	static Buffer make(const Context& context, const cl_mem_flags& flags, const nr_uint& count, cl_status* status = nullptr)
	{
		return make<T>(context, flags, count, nullptr, status);
	}

	Buffer();

	Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, cl_status* err = nullptr);

	Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, void* data = nullptr, cl_status* error = nullptr);

	explicit Buffer(const cl_mem& buffer, const nr_bool retain = false);

	Buffer(const Buffer& other);

	Buffer(Buffer&& other);

	Buffer& operator=(const Buffer& other);

	Buffer& operator=(Buffer&& other);

	operator cl_mem() const;

	const cl_mem& get() const;

    /**
     * @brief Aquire the size (in bytes) of the buffer in device memory
     * @param[out] err internal OpenCL call error status
	 * @return the buffer size (in bytes)
     */
	nr_size getBufferSize(cl_status* err = nullptr) const;

	/**
	 * @brief Get the amount of elements of type T which the buffer can hold
	 * @tparam T element type
	 * @param[out] err internal OpenCL call error status
	 * @return buffer capacity for elements of type T
	 */
	template <typename T>
	nr_size getBufferCount(cl_status* err = nullptr) const
	{
		return getBufferSize(err) / sizeof(T);
	}
};

}