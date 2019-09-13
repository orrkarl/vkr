/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style Kernel class 
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <array>
#include <tuple>
#include <utility>

#include "Wrapper.h"

namespace nr
{

class Module;

/**
 * @brief Host representation of an OpenCL device function which is callable from the host
 * 
 * A kernel is thie interface which holds the data required to execute device code. 
 * The execution itself, however, is done with a CommandQueue
 * @note wraps cl_kernel
 * @see CommandQueue
 */
class NRAPI Kernel : public Wrapper<cl_kernel>
{
public:
	/**
	 * @brief Constructs a null kernel
	 *
	 */
	Kernel();

	/**
	 * @brief Convertes a raw OpenCL kernel to C++ wrapper
	 *
	 * This method allowes the class to "take ownership" of the lower OpenCL type; It may retain (increase the reference count) of the object
	 * @param kernel object to own
	 * @param retain should the reference count for the object be incremented
	 */
	Kernel(const cl_kernel& kernel, const nr_bool retain, cl_status& status);

	/**
	 * @brief Constructs a kenrel object
	 *
	 * This function searches for a kernel with a given name in the given module, and creates the kernel if the name is found.
	 * If the kernel is not found within the module, a null kernel will be constructed
	 * @note wraps clCreateKernel
	 * @note refer to the OpenCL clCreateKernel documentation for detailed explenation of the possible status codes
	 * @param module OpenCL module containing the kernel
	 * @param name kernel name in the module
	 * @param[out] err internal OpenCL call status
	 */
	Kernel(const Module& module, const string& name, cl_status& err);

	/**
	 * @brief Constructs a kenrel object
	 *
	 * This function searches for a kernel with a given name in the given module, and creates the kernel if the name is found.
	 * If the kernel is not found within the module, a null kernel will be constructed
	 * @note wraps clCreateKernel
	 * @note refer to the OpenCL clCreateKernel documentation for detailed explenation of the possible status codes
	 * @param module OpenCL module containing the kernel
	 * @param name kernel name in the module
	 * @param[out] err internal OpenCL call status
	 */
	Kernel(const Module& module, const char* name, cl_status& err);

	Kernel(const Kernel& other);

	Kernel(Kernel&& other);

	Kernel& operator=(const Kernel& other);

	Kernel& operator=(Kernel&& other);

	/**
	 * @brief provides access to the underlying object
	 *
	 * @return cl_kernel underlying OpenCL object
	 */
	operator cl_kernel() const;

	/**
	 * @brief access the underlying opencl kernel
	 *
	 * @return cl_kernel raw opencl kernel
	 */
	cl_kernel get() const;

	/**
	 * @brief sets one of the internal kernels arguments
	 *
	 * This method takes types which are not OpenCL wrappers and are not raw pointers.
	 * The argument will be copied raw as a kernel argument
	 * @note wraps clSetKernelArg
	 * @note Refer to OpenCL clSetKernelArg documentation for detailed explenation of return status
	 * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
	 * @param   index   kernel parameter index
	 * @param   value   argument to be passed to the kernel
	 * @return  cl_status internal OpenCL call error status
	 */
	template<typename T>
	typename std::enable_if<!std::is_pointer<T>::value && !std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const nr_uint index, const T& value)
	{
		return clSetKernelArg(object, index, sizeof(T), &value);
	}

	/**
	 * @brief sets one of the internal kernels arguments
	 *
	 * This method takes OpenCL memory wrapper types, and will pass the internel memory object they contain to the kernel
	 * @note wraps clSetKernelArg
	 * @note Refer to OpenCL clSetKernelArg documentation for detailed explenation of return status
	 * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
	 * @param   index   kernel parameter index
	 * @param   value   argument to be passed to the kernel
	 * @return  cl_status internal OpenCL call error status
	 */
	template<typename T>
	typename std::enable_if<!std::is_pointer<T>::value&& std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const nr_uint index, const T& value)
	{
		return clSetKernelArg(object, index, sizeof(T), &(value.get()));
	}

	/**
	 * @brief sets one of the internal kernels arguments
	 *
	 * This method provides a more 'low level', although less type safe access to the kerenl parameters.
	 * It will copy 'size' bytes from given address as a kernel argument
	 * @note wraps clSetKernelArg
	 * @note Refer to OpenCL clSetKernelArg documentation for detailed explenation of return status
	 * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
	 * @param   index   kernel parameter index
	 * @param   size    kernel parameter size
	 * @param   value   argument to be passed to the kernel
	 * @return  internal OpenCL call error status
	 */
	cl_status setArg(const nr_uint index, const nr_uint size, const void* value)
	{
		return clSetKernelArg(object, index, size, value);
	}

private:
	explicit Kernel(const cl_kernel kernel);
};

/**
 * @brief Typesafe wrapper for Kernel
 * 
 * This class gives a more type safe interface - the kernel argument types and their indices are specified in the kernel class, thus calling to setArg
 * is safer
 * @tparam Args kernel invocation argument types
 */
template <typename... Args>
class TypesafeKernel : private Kernel
{
private:
	template <nr_uint Index>
	using Type = typename std::tuple_element<Index, std::tuple<Args...>>::type;

#ifdef _DEBUG
protected:
	std::array<nr_bool, sizeof...(Args)> setIndices{ false };
#endif

public:
	/**
	 * @brief Construct a Typesafe kernel
	 *
	 * @note delegates to Kernel::Kernel(const Module& module, const char* name, cl_status& err)
	 * @param module OpenCL module containing the kernel
	 * @param name kernel name in the module
	 * @param err internal OpenCL call status
	 */
	TypesafeKernel(const Module& module, const char* name, cl_status& err)
		: Kernel(module, name, err)
	{
	}

	/**
	 * @brief Construct a Typesafe kernel
	 *
	 * @note delegates to Kernel::Kernel(const Module& module, const char* name, cl_status& err)
	 * @param module OpenCL module containing the kernel
	 * @param name kernel name in the module
	 * @param err internal OpenCL call status
	 */
	TypesafeKernel(const Module& module, const string& name, cl_status& err)
		: Kernel(module, name, err)
	{
	}

    /**
     * @brief Construct a null Typesafe kernel
     * 
     * @note delegates to Kernel::Kernel()
     */
	TypesafeKernel()
		: Kernel()
	{
	}

    /**
     * @brief Set the kernel argument at given index
     * 
     * @note delegates to Kernel::setArg(const nr_uint index, const T& value)
     * @tparam Index kernel argument index
     * @param arg kernel argument
     * @return cl_status internal OpenCL call status
     */
	template <nr_uint Index>
	cl_status setArg(const Type<Index>& arg)
	{
#ifdef _DEBUG
		setIndices[Index] = true;
#endif
		return Kernel::setArg(Index, arg);
	}

	using Kernel::get;

	using Kernel::operator cl_kernel;
};

}
