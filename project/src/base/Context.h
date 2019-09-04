/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style device Context class
 * @version 0.6.0
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <vector>
#include <functional>

#include "Device.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief bare wrapper for cl_context, for a more comfortable interface.
 * 
 * The OpenCL context, behind the stages, is a container for many of the basic OpenCL components, such as buffers or programs.
 */
class NRAPI Context : public Wrapper<cl_context>
{
public:
    /**
     * @brief Construct a null context
     * 
     */
    Context();

    /**
	 * @brief Convertes a raw OpenCL context to C++ wrapper
	 * 
	 * @param context object to own
	 * @param retain should the reference count for the object be incremented
	 */
    Context(const cl_context& context, const nr_bool retain, cl_status& status);

    Context(const Context& other);

    Context(Context&& other);

    /**
     * @brief Construct a context object, bound to some devices with certain properties
     * 
     * Refer to the OpenCL clCreateContext documentation for detailed explenation of the context properties
     * @note wraps clCreateContext
     * @tparam T user data callback type
     * @param properties context properties
     * @param devices devices bound to the context
     * @param callback context message callback
     * @param userData data to be passed to the context each time it's called
     * @param[out] err internal OpenCL error status
     */
    template<typename T>
    Context(
        const cl_context_properties* properties, 
        std::vector<Device>& devices, 
        const std::function<void(const char* errinfo, const void* clinfo, size_t size, T* userData)> callback,
        T* userData,
        cl_status& err)
        : Wrapped(
            clCreateContext(
                properties, 
                devices.size(), static_cast<cl_device_id*>(&devices.front()), 
                static_cast<void(CL_CALLBACK *)(const char*, const void*, size_t, void*)>(
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) userData);}),
                userData, 
                &err))
    {
    }

    /**
     * @brief Construct a context object, bound to some devices with certain properties
     * 
     * Refer to the OpenCL clCreateContext documentation for detailed explenation of the context properties
     * @note wraps clCreateContext
     * @param properties context properties
     * @param devices devices bound to the context
     * @param[out] err internal OpenCL error status
     */
    Context(
        const cl_context_properties* properties, 
        std::vector<Device>& devices, 
        cl_status& err);

    /**
     * @brief Construct a new Context object, bound to all available devices of a ceratin type
     * 
     * Refer to the OpenCL clCreateContext documentation for detailed explenation of the context properties
     * @note wraps clCreateContextFromType
     * @tparam T user data callback type
     * @param properties context properties
     * @param deviceType bound devices type
     * @param callback context message callback
     * @param userData data to be passed to the context each time it's called
     * @param[out] err internal OpenCL error status
     */
    template<typename T>
    Context(
        const cl_context_properties* properties, 
        cl_device_type deviceType, 
        const std::function<void(const char* errinfo, const void* clinfo, size_t size, T* userData)> callback,
        T* userData,
        cl_status& err)
        : Wrapped(
            clCreateContextFromType(
                properties, 
                deviceType,
                static_cast<void(CL_CALLBACK *)(const char*, const void*, size_t, void*)>(
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) userData);}),
                userData, 
                &err))
    {
    }

    /**
     * @brief an 'easy to use' constructor for a Context. Using the given properties and collects all of the Devices of a certain type
     * 
     * Refer to the OpenCL clCreateContext documentation for detailed explenation of the context properties
     * @note wraps clCreateContextFromType
     * @param       properties  context properties
     * @param       deviceType  type of collected devices. Will default to GPU.
     * @param[out]  err OpenCL internal call error status
     */
    Context(
        const cl_context_properties* properties, 
        cl_device_type deviceType,          
        cl_status& err);

	/**
	 * @brief an 'easier to use' constructor for a Context. Using the given properties and collects all of the GPU devices
	 *
	 * Refer to the OpenCL clCreateContext documentation for detailed explenation of the context properties
	 * @note wraps clCreateContextFromType
	 * @see Context::Context(const cl_context_properties* properties, cl_device_type deviceType, cl_status& err)
	 * @param       properties  context properties
	 * @param       deviceType  type of collected devices. Will default to GPU.
	 * @param[out]  err OpenCL internal call error status
	 */
	Context(
		const cl_context_properties* properties,
		cl_status& err);

    Context& operator=(const Context& other);

    Context& operator=(Context&& other);

    /**
     * @brief provides access to the underlying OpenCL context
     * 
     * @return cl_context underlying context
     */
    operator cl_context() const;
};

}