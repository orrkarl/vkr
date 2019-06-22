#pragma once

#include "../general/predefs.h"

#include "Wrapper.h"

namespace nr
{

class NR_SHARED Context : public Wrapper<cl_context>
{
// Functions and Constructors
public:
    static makeDefault(Context provided)
    {
        defaultContext = provided;
    }

    static getDefault()
    {
        return defaultContext;
    }

    Context()
        : Wrapped()
    {
    }

    explicit Context(const cl_context& Context, const NRbool retain = false)
        : Wrapped(Context, retain)
    {
    }

    Context(const Context& other)
        : Wrapped(other)
    {
    }

    Context(Context&& other)
        : Wrapped(other)
    {
    }

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
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) uesrData)}),
                userData, 
                err))
    {
    }

    Context(
        const cl_context_properties* properties, 
        std::vector<Device>& devices, 
        cl_status* err = nullptr)
        : Wrapped(
            clCreateContext(
                properties, 
                devices.size(), static_cast<cl_device_id*>(&devices.front()), 
                nullptr, nullptr, 
                err))
    {
    }

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
                    [callback](const char* errinfo, const void* clinfo, size_t size, void* userData){callback(errinfo, clinfo, size, (T*) uesrData)}),
                userData, 
                err))
    {
    }

    Context(
        const cl_context_properties* properties, 
        cl_device_type deviceType = CL_DEVICE_TYPE_GPU,          
        cl_status* err = nullptr)
        : Wrapped(
            clCreateContextFromType(
                properties, 
                deviceType,
                nullptr, nullptr, 
                err))
    {
    }

    Context& operator=(const Context& other)
    {
        return Wrapped::operator=(other);
    }

    Context& operator=(Context&& other)
    {
        return Wrapped::operator=(other);
    }

    operator cl_context() const 
    {
        return object;
    }

// Fields
private:
    static Context defaultContext;
};

}