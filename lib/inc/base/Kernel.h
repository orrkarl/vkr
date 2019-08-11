/**
 * @file Kernel.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping OpenCL cl_kernel
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

#include <array>
#include <tuple>
#include <utility>

#include "Wrapper.h"

namespace nr
{

class Module;

/**
 * @brief Wrapper class for cl_kernel - the OpenCL handle for executing device side code.
 * 
 */
class NRAPI Kernel : public Wrapper<cl_kernel>
{
public:
    Kernel();

    explicit Kernel(const cl_kernel& kernel, const nr_bool retain = false);

    Kernel(const Module& module, const string& name, cl_status* err = nullptr);

	Kernel(const Module& module, const char* name, cl_status* err = nullptr);

    Kernel(const Kernel& other);

    Kernel(Kernel&& other);

    Kernel& operator=(const Kernel& other);

    Kernel& operator=(Kernel&& other);

    operator cl_kernel() const;

	cl_kernel get() const;

    /**
     * @brief sets one of the internal kernels arguments
     * 
     * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
     * @param   index   kernel parameter index
     * @param   value   argument to be passed to the kernel
     * @return  internal OpenCL call error status
     */
    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value && !std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const nr_uint index, const T& value)
    {
        return clSetKernelArg(object, index, sizeof(T), &value);
    }

    /**
     * @brief sets one of the internal kernels arguments
     * 
     * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
     * @param   index   kernel parameter index
     * @param   value   argument to be passed to the kernel
     * @return  internal OpenCL call error status
     */
    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value && std::is_base_of<Wrapper<cl_mem>, T>::value, cl_status>::type setArg(const nr_uint index, const T& value)
    {
        return clSetKernelArg(object, index, sizeof(T), &(value.get()));
    }
    
    /**
     * @brief sets one of the internal kernels arguments
     * @par
     * This method provides a more 'low level', although less type safe access to the kerenl parameters.
     * 
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
};

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
	TypesafeKernel(const Module& module, const char* name, cl_status* err = nullptr)
		: Kernel(module, name, err)
	{
	}

	TypesafeKernel()
		: Kernel()
	{
	}

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
