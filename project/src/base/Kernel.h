#pragma once

#include "../predefs.h"

#include <array>
#include <tuple>
#include <utility>

#include "Views.h"
#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(Kernel);

class Module;

/**
 * @brief Host representation of an OpenCL device function which is callable from the host
 *
 * A kernel is the interface which holds the data required to execute device code.
 * The execution itself, however, is done with a CommandQueue
 *
 * @note wraps cl_kernel
 * @see CommandQueue
 */
class Kernel {
public:
    /**
     * @brief Constructs a kernel object
     *
     * This function searches for a kernel with a given name in the given module, and creates the kernel if
     * the name is found. If the kernel is not found within the module, a null kernel will be constructed
     * @note wraps clCreateKernel
     * @note refer to the OpenCL clCreateKernel documentation for detailed explenation of the possible status
     * codes
     * @param module OpenCL module containing the kernel
     * @param name kernel name in the module
     */
    Kernel(Module& module, const std::string& name);

    /**
     * @brief Constructs a kernel object
     *
     * This function searches for a kernel with a given name in the given module, and creates the kernel if
     * the name is found. If the kernel is not found within the module, a null kernel will be constructed
     * @note wraps clCreateKernel
     * @note refer to the OpenCL clCreateKernel documentation for detailed explenation of the possible status
     * codes
     * @param module OpenCL module containing the kernel
     * @param name kernel name in the module
     */
    Kernel(Module& module, const char* name);

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
    template <typename T>
    typename std::enable_if<!std::is_pointer<T>::value, void>::type setArg(U32 index, const T& value)
    {
        return setArg(index, sizeof(T), &value);
    }

    /**
     * @brief sets one of the internal kernels arguments
     *
     * This method takes OpenCL memory wrapper types, and will pass the internal memory object they contain to
     * the kernel
     * @note wraps clSetKernelArg
     * @note Refer to OpenCL clSetKernelArg documentation for detailed explenation of return status
     * @tparam  T       type of kernel parameter (has to be of the same size as device side type)
     * @param   index   kernel parameter index
     * @param   value   argument to be passed to the kernel
     * @return  cl_status internal OpenCL call error status
     */
    void setArg(const U32 index, MemoryView& value)
    {
        cl_mem memoryObj = value.rawHandle();
        return setArg(index, sizeof(memoryObj), &memoryObj);
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
    void setArg(U32 index, size_t size, const void* value)
    {
        auto status = clSetKernelArg(m_object, index, size, value);
        if (status != CL_SUCCESS) {
            throw CLApiException(status, "could not set kernel argument");
        }
    }

private:
    friend class CommandQueue;

    struct KernelTraits {
        static constexpr auto release = clReleaseKernel;
        using Type = cl_kernel;
    };

    cl_kernel rawHandle() const;

    UniqueWrapper<KernelTraits> m_object;
};

/**
 * @brief Typesafe wrapper for Kernel
 *
 * This class gives a more type safe interface - the kernel argument types and their indices are specified in
 * the kernel class, thus calling to setArg is safer
 * @tparam Args kernel invocation argument types
 */
template <typename... Args> class TypesafeKernel : private Kernel {
private:
    template <nr_uint Index> using Type = typename std::tuple_element<Index, std::tuple<Args...>>::type;

#ifdef _DEBUG
protected:
    std::array<nr_bool, sizeof...(Args)> setIndices { false };
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
    template <nr_uint Index> cl_status setArg(const Type<Index>& arg)
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
