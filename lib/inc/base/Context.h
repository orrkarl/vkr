#pragma once

#include "../general/predefs.h"

#include <vector>
#include <functional>

#include "Device.h"
#include "Wrapper.h"

namespace nr
{

class NRAPI Context : public Wrapper<cl_context>
{
// Functions and Constructors
public:
    static void makeDefault(const Context& provided);

    static Context getDefault();

    Context();

    explicit Context(const cl_context& context, const nr_bool retain = false);

    Context(const Context& other);

    Context(Context&& other);

    template<typename T>
    Context(
        const cl_context_properties* properties, 
        std::vector<Device>& devices, 
        const std::function<void(const char* errinfo, const void* clinfo, size_t size, T* userData)> callback,
        T* userData,
        cl_status* err = nullptr)
        : Wrapped(
            clCreateContext(
                properties, 
                devices.size(), static_cast<cl_device_id*>(&devices.front()), 
                static_cast<void(CL_CALLBACK *)(const char*, const void*, size_t, void*)>(
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) userData);}),
                userData, 
                err))
    {
    }

    Context(
        const cl_context_properties* properties, 
        std::vector<Device>& devices, 
        cl_status* err = nullptr);

    template<typename T>
    Context(
        const cl_context_properties* properties, 
        cl_device_type deviceType, 
        const std::function<void(const char* errinfo, const void* clinfo, size_t size, T* userData)> callback,
        T* userData,
        cl_status* err = nullptr)
        : Wrapped(
            clCreateContextFromType(
                properties, 
                deviceType,
                static_cast<void(CL_CALLBACK *)(const char*, const void*, size_t, void*)>(
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) userData);}),
                userData, 
                err))
    {
    }

    Context(
        const cl_context_properties* properties, 
        cl_device_type deviceType = CL_DEVICE_TYPE_GPU,          
        cl_status* err = nullptr);

    Context& operator=(const Context& other);

    Context& operator=(Context&& other);

    operator cl_context() const;

// Fields
private:
    static Context defaultContext;
};

}