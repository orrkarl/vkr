#pragma once

#include "../general/predefs.h"

#include "Buffer.h"
#include "Context.h"
#include "Event.h"
#include "Kernel.h"
#include "Wrapper.h"

#include <array>

namespace nr
{

class NRAPI CommandQueue : public Wrapper<cl_command_queue>
{
public:
    static CommandQueue getDefault();
    
    static void makeDefault(const CommandQueue& queue);

    CommandQueue();

    explicit CommandQueue(const cl_command_queue& commandQueue, const nr_bool retain = false);

    CommandQueue(const CommandQueue& other);

    CommandQueue(CommandQueue&& other);

    CommandQueue(Context context, Device device, cl_command_queue_properties properties, cl_status* err = nullptr);

    CommandQueue(Device device, cl_command_queue_properties properties, cl_status* err = nullptr);

    CommandQueue(cl_command_queue_properties properties, cl_status* err = nullptr);

    CommandQueue& operator=(const CommandQueue& other);

    CommandQueue& operator=(CommandQueue&& other);

    operator cl_command_queue() const;

    cl_status flush();

    cl_status await();

    cl_status finish();
    
    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer<T>& buffer, nr_bool block, nr_ulong count, T* data, const std::vector<Event>& wait, nr_ulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueReadBuffer(
            object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
            wait.size(), (const cl_event*) &wait.front(), (cl_event*) notify);
    }

    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer<T>& buffer, nr_bool block, nr_ulong count, T* data, nr_ulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueReadBuffer(
                object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
                0, nullptr, (cl_event*) notify);
    }

    template<typename T>
    cl_status enqueueBufferWriteCommand(const Buffer<T>& buffer, nr_bool block, nr_ulong count, T* data, const std::vector<Event>& wait, nr_ulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueWriteBuffer(
            object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
            wait.size(), (const cl_event*) &wait.front(), (cl_event*) notify);
    }

    template<typename T>
    cl_status enqueueBufferWriteCommand(const Buffer<T>& buffer, nr_bool block, nr_ulong count, T* data, nr_ulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueWriteBuffer(
                object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
                0, nullptr, (cl_event*) notify);
    }

    template<nr_uint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const std::array<size_t, dim>& global, const std::array<size_t, dim>& local,
        const std::array<size_t, dim>& offset, const std::vector<Event>& wait, Event* notify = nullptr)
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, &offset.front(), &global.front(), &local.front(), wait.size(), &wait.front(), (cl_event*) notify);
    }

    template<nr_uint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const std::array<size_t, dim>& global, const std::array<size_t, dim>& local,
        const std::array<size_t, dim>& offset = std::array<size_t, dim>{}, Event* notify = nullptr)
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, &offset.front(), &global.front(), &local.front(), 0, nullptr, (cl_event*) notify);
    }

    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer<T>& buffer, 
        const T& value, const nr_ulong& count, 
        const std::vector<Event>& wait, 
        const nr_uint& offset = 0, Event* notify = nullptr)
    {
        return clEnqueueFillBuffer(object, buffer, value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, wait.size(), &wait.front(), (cl_event*) notify);
    }

    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer<T>& buffer, 
        const T& value, const nr_ulong& count, 
        const nr_uint& offset = 0, Event* notify = nullptr)
    {
        return clEnqueueFillBuffer(object, buffer, value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, 0, nullptr, (cl_event*) notify);
    }

// Fields
private:
    static CommandQueue defaultQueue;
};

}
