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

class NR_SHARED CommandQueue : public Wrapper<cl_command_queue>
{
public:
    static CommandQueue getDefault();
    
    static void makeDefault(const CommandQueue& queue);

    CommandQueue();

    explicit CommandQueue(const cl_command_queue& commandQueue, const NRbool retain = false);

    CommandQueue(const CommandQueue& other);

    CommandQueue(CommandQueue&& other);

    CommandQueue(Context& context, Device& device, cl_command_queue_properties& properties, cl_status* err = nullptr);

    CommandQueue(Device& device, cl_command_queue_properties& properties, cl_status* err = nullptr);

    CommandQueue(cl_command_queue_properties& properties, cl_status* err = nullptr);

    CommandQueue& operator=(const CommandQueue& other);

    CommandQueue& operator=(CommandQueue&& other);

    operator cl_command_queue() const;

    cl_status flush();

    cl_status await();

    cl_status finish();
    
    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer<T>& buffer, NRbool block, NRulong count, T* data, const std::vector<Event>& wait, NRulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueReadBuffer(
            object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
            wait.size(), (const cl_event*) &wait.front(), notify);
    }

    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer<T>& buffer, NRbool block, NRulong count, T* data, NRulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueReadBuffer(
                object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
                0, nullptr, notify);
    }

    template<typename T>
    cl_status enqueueBufferWriteCommand(const Buffer<T>& buffer, NRbool block, NRulong count, T* data, const std::vector<Event>& wait, NRulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueWriteBuffer(
            object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
            wait.size(), (const cl_event*) &wait.front(), notify);
    }

    template<typename T>
    cl_status enqueueBufferWriteCommand(const Buffer<T>& buffer, NRbool block, NRulong count, T* data, NRulong offset = 0, Event* notify = nullptr)
    {
        return clEnqueueWriteBuffer(
                object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
                0, nullptr, notify);
    }

    template<NRuint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const std::array<NRuint, dim>& global, const std::array<NRuint, dim>& local,
        const std::array<NRuint, dim>& offset, const std::vector<Event>& wait, Event* notify = nullptr)
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, &offset.front(), &global.front(), &local.front(), wait.size(), &wait.front(), notify);
    }

    template<NRuint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const std::array<NRuint, dim>& global, const std::array<NRuint, dim>& local,
        const std::array<NRuint, dim>& offset = std::array<NRuint, dim>{}, Event* notify = nullptr)
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, &offset.front(), &global.front(), &local.front(), 0, nullptr, notify);
    }

    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer<T>& buffer, 
        const T& value, const NRulong& count, 
        const std::vector<Event>& wait, 
        const NRuint& offset = 0, Event* notify = nullptr)
    {
        return clEnqueueFillBuffer(object, buffer, value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, wait.size(), &wait.front(), notify);
    }

    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer<T>& buffer, 
        const T& value, const NRulong& count, 
        const NRuint& offset = 0, Event* notify = nullptr)
    {
        return clEnqueueFillBuffer(object, buffer, value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, 0, nullptr, notify);
    }

// Fields
private:
    static CommandQueue defaultQueue;
};

}
