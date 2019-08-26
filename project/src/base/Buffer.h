/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style buffer class
 * @version 0.6.0
 * @date 2019-08-26
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"
#include "Wrapper.h"
#include "Context.h"

namespace nr
{

/**
 * @brief Host representation of a plain OpenCL memory pointer
 * 
 * This class represents the most simple memory scheme available. Think of this class as a "Device pointer". 
 * Internally, this class wraps cl_mem and buffer clCreateBuffer
 * @note Host access (read/write) to the underlying data within the buffer is done with a CommandQueue
 * @see CommandQueue
 */
class NRAPI Buffer : public Wrapper<cl_mem>
{
public:
	/**
	 * @brief More typesafe method to create buffers
	 * 
	 * Creates a buffer whose size is count * sizeof(T), i.e saves enough place for 'count' elements of type T
	 * @tparam T expected buffer underlying datatype
	 * @param context parent context of this buffer
	 * @param flags memory access flags
	 * @param count amount of elements this buffer should hold
	 * @param data host pointer, will be used according to flags
	 * @param[out] status function success status
	 * @return Buffer a size-fitting buffer
	 */
	template <typename T>
	static Buffer make(const Context& context, const cl_mem_flags& flags, const nr_uint& count, T* data, cl_status* status = nullptr)
	{
		return Buffer(context, flags, static_cast<nr_size>(count * sizeof(T)), data, status);
	}

	/**
	 * @brief More typesafe method to create buffers
	 * 
	 * Creates a buffer whose size is count * sizeof(T), i.e saves enough place for 'count' elements of type T. Doesn't expect any host pointer
	 * @tparam T expected buffer underlying datatype
	 * @param context parent context of this buffer
	 * @param flags memory access flags. Do not pass CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR, as no host pointer is passed in this version
	 * @param count amount of elements this buffer should hold
	 * @param data host pointer, will be used according to flags
	 * @param[out] status function success status
	 * @return Buffer a size-fitting buffer
	 */
	template <typename T>
	static Buffer make(const Context& context, const cl_mem_flags& flags, const nr_uint& count, cl_status* status = nullptr)
	{
		return make<T>(context, flags, count, nullptr, status);
	}

	/**
	 * @brief Construct a new empty Buffer object; the OpenCL equivelant of nullptr
	 * 
	 */
	Buffer();

	/**
	 * @brief Construct a new Buffer object of given size; wraps clCreateBuffer
	 * 
	 * Refer to the OpenCL clCreateBuffer documentation for detailed explenation of the memory flags and error status
	 * @param context parent context of this buffer
	 * @param flags memory access flags. Do not pass CL_MEM_COPY_HOST_PTR or CL_MEM_USE_HOST_PTR, as no host pointer is passed in this version
	 * @param size the buffer size (in bytes)
	 * @param[out] err function success status
	 */
	Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, cl_status* err = nullptr);

	/**
	 * @brief Construct a new Buffer object of given size; wraps clCreateBuffer
	 * 
	 * Refer to the OpenCL clCreateBuffer documentation for detailed explenation of the memory flags and error status
	 * @param context parent context of this buffer
	 * @param flags memory access flags
	 * @param size the buffer size (in bytes)
	 * @param[out] err function success status
	 */
	Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, void* data = nullptr, cl_status* error = nullptr);

	/**
	 * @brief Convertes a raw OpenCL buffer to C++ wrapper
	 * 
	 * @param buffer object to own
	 * @param retain should the reference count for the object be incremented
	 */
	explicit Buffer(const cl_mem& buffer, const nr_bool retain = false);

	Buffer(const Buffer& other);

	Buffer(Buffer&& other);

	Buffer& operator=(const Buffer& other);

	Buffer& operator=(Buffer&& other);

	/**
	 * @brief access the underlying opencl buffer
	 * 
	 * @return cl_mem raw opencl buffer
	 */
	operator cl_mem() const;

	/**
	 * @brief access the underlying opencl buffer
	 * 
	 * @return const cl_mem& raw opencl buffer
	 */
	const cl_mem& get() const;

    /**
     * @brief Aquire the size (in bytes) of the buffer in device memory
	 * 
     * @param[out] err internal OpenCL call error status
	 * @return nr_size the buffer size (in bytes)
     */
	nr_size size(cl_status* err = nullptr) const;

	/**
	 * @brief Get the amount of elements of type T which the buffer can hold
	 * 
	 * @tparam T element type
	 * @param[out] err internal OpenCL call error status
	 * @return nr_size buffer capacity for elements of type T
	 */
	template <typename T>
	nr_size count(cl_status* err = nullptr) const
	{
		return size(err) / sizeof(T);
	}
};

}