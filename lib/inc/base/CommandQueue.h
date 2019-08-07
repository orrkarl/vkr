/**
 * @file CommandQueue.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping OpenCL command queue, used to enqueue command to the device. 
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

#include "Buffer.h"
#include "Context.h"
#include "Event.h"
#include "Kernel.h"
#include "Wrapper.h"

#include <array>
#include <typeinfo>

namespace nr
{

class CommandQueue;

template <typename Consumer>
class Dispatch
{
public:
	cl_status operator()(nr::CommandQueue* q)
	{
		return static_cast<Consumer*>(this)->consume(q);
	}
};

template <nr_uint dim>
struct NDRange {};

#pragma pack(push, 1)

template <>
struct NDRange<1>
{
	union
	{
		nr_size x;
		nr_size data[1];
	};
};

template <>
struct NDRange<2>
{
	union
	{
		struct
		{
			nr_size x;
			nr_size y;
		};
		nr_size data[2];
	};
};

template <>
struct NDRange<3>
{
	union
	{
		struct
		{
			nr_size x;
			nr_size y;
			nr_size z;
		};
		nr_size data[3];
	};
};

#pragma pack(pop)

template <nr_uint dim>
struct NDExecutionRange
{
	NDRange<dim> global, local;
};

/**
 * @brief Wrapper class for OpenCl command queue, allowing for a more type safe and 'cpp-esque' interface.
 * 
 * @par
 * Every interaction with the device (e.g buffer copy, kernel execution) has to be enqueued through this class
 * 
 */
class NRAPI CommandQueue : public Wrapper<cl_command_queue>
{
public:
    CommandQueue();

    explicit CommandQueue(const cl_command_queue& commandQueue, const nr_bool retain = false);

    CommandQueue(const CommandQueue& other);

    CommandQueue(CommandQueue&& other);

    CommandQueue(Context context, Device device, cl_command_queue_properties properties, cl_status* err = nullptr);

    CommandQueue& operator=(const CommandQueue& other);

    CommandQueue& operator=(CommandQueue&& other);

    operator cl_command_queue() const;

    /**
     * @brief ensures that all of the waiting commands will be transfered to the device side queue
     * @return internal OpenCL call error status
     */
    cl_status flush();

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @see finish
     * @return internal OpenCL call error status
     */
    cl_status await();

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @return internal OpenCL call error status
     */
    cl_status finish();
    
    /**
     * @brief enqueues a buffer read command - moving data from a device buffer to host memory
     * 
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       block   should the function block (wait until the data read is completed)
     * @param       count   counting how many elements of T will be copied from the buffer
     * @param[out]  data    destination for the memory copy operation
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer& buffer, nr_bool block, nr_uint count, T* data, const std::vector<Event>& wait, nr_uint offset = 0, Event* notify = nullptr) const
    {
        return clEnqueueReadBuffer(
            object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
            wait.size(), (const cl_event*) wait.data(), (cl_event*) notify);
    }

    /**
     * @brief enqueues a buffer read command - moving data from a device buffer to host memory
     * 
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       block   should the function block (wait until the data read is completed)
     * @param       count   counting how many elements of T will be copied from the buffer
     * @param[out]  data    destination for the memory copy operation
     * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferReadCommand(const Buffer& buffer, nr_bool block, nr_uint count, T* data, nr_uint offset = 0, Event* notify = nullptr) const
    {
        return clEnqueueReadBuffer(
                object, buffer, block, offset * sizeof(T), count * sizeof(T), data, 
                0, nullptr, (cl_event*) notify);
    }

	/**
	 * @brief enqueues a buffer read command - reading the entire buffer to host memory
	 *
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param[out]  data    destination for the memory copy operation
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferReadCommand(const Buffer& buffer, nr_bool block, T* data, Event* notify = nullptr) const
	{
		return clEnqueueReadBuffer(
			object, buffer, block, 0, buffer.size(), data,
			0, nullptr, (cl_event*)notify);
	}

	/**
	 * @brief enqueues a buffer write command - moving data from host memory to a device buffer
	 *
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param       count   counting how many elements of T will be copied from the buffer
	 * @param       data    source for memory copy operation
	 * @param       wait    list of events that have to complete before this command will begin execution
	 * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferWriteCommand(const Buffer& buffer, nr_bool block, nr_uint count, T* data, const std::vector<Event>& wait, nr_uint offset = 0, Event * notify = nullptr) const
	{
		return clEnqueueWriteBuffer(
			object, buffer, block, offset * sizeof(T), count * sizeof(T), data,
			wait.size(), (const cl_event*)wait.data(), (cl_event*)notify);
	}

	/**
	 * @brief enqueues a buffer write command - moving data from host memory to a device buffer
	 *
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param       count   counting how many elements of T will be copied from the buffer
	 * @param       data    source for memory copy operation
	 * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferWriteCommand(const Buffer& buffer, nr_bool block, nr_uint count, T* data, nr_uint offset = 0, Event * notify = nullptr) const
	{
		return clEnqueueWriteBuffer(
			object, buffer, block, offset * sizeof(T), count * sizeof(T), data,
			0, nullptr, (cl_event*)notify);
	}

    /**
     * @brief enqueues a kernel command - submits a kernel to the device
     * 
     * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @param       kernel  kernel to be submitted
     * @param       global  global execution size
     * @param       local   local execution size
     * @param       offset  local execution offset
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<nr_uint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const NDRange<dim>& global, const NDRange<dim>& local,
        const NDRange<dim>& offset, const std::vector<Event>& wait, Event* notify = nullptr) const
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, offset.data, global.data, local.data, wait.size(), wait.data(), (cl_event*) notify);
    }

    /**
     * @brief enqueues a kernel command - submits a kernel to the device
     * 
     * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @param       kernel  kernel to be submitted
     * @param       global  global execution size
     * @param       local   local execution size
     * @param       offset  local execution offset
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<nr_uint dim>
    typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
        const Kernel& kernel, 
        const NDRange<dim>& global, const NDRange<dim>& local,
        const NDRange<dim>& offset = NDRange<dim>{}, Event* notify = nullptr) const
    {
        return clEnqueueNDRangeKernel(object, kernel, dim, offset.data, global.data, local.data, 0, nullptr, (cl_event*) notify);
    }

	/**
	 * @brief enqueues a kernel command - submits a kernel to the device
	 *
	 * @tparam      dim     execution dimension - has to be between 1, 2 or 3
	 * @param       kernel  kernel to be submitted
	 * @param       range	spcification of the kernel execution range
	 * @param       offset  local execution offset
	 * @param       wait    list of events that have to complete before this command will begin execution
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      internal OpenCL error status
	 */
	template<nr_uint dim>
	typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
		const Kernel & kernel,
		const NDExecutionRange<dim>& range,
		const NDRange<dim> & offset, const std::vector<Event> & wait, Event * notify = nullptr) const
	{
		return clEnqueueNDRangeKernel(object, kernel, dim, offset.data, range.global.data, range.local.data, wait.size(), wait.data(), (cl_event*)notify);
	}

	/**
	 * @brief enqueues a kernel command - submits a kernel to the device
	 *
	 * @tparam      dim     execution dimension - has to be between 1, 2 or 3
	 * @param       kernel  kernel to be submitted
	 * @param       range	spcification of the kernel execution range
	 * @param       offset  local execution offset
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      internal OpenCL error status
	 */
	template<nr_uint dim>
	typename std::enable_if<1 <= dim && dim <= 3, cl_status>::type enqueueKernelCommand(
		const Kernel & kernel,
		const NDExecutionRange<dim>& range,
		const NDRange<dim> & offset = NDRange<dim>{}, Event * notify = nullptr) const
	{
		return clEnqueueNDRangeKernel(object, kernel, dim, offset.data, range.global.data, range.local.data, 0, nullptr, (cl_event*)notify);
	}

	/**
	 * @brief enqueues a kernel command to the device
	 *
	 * @tparam      T			CRTP dispatch type
	 * @param       dispatch	dispatch to execute
	 * @return      internal	OpenCL error status
	 */
	template <typename T>
	cl_status enqueueDispatchCommand(const Dispatch<T>& dispatch)
	{
		return dispatch(this);
	}

    /**
     * @brief enqueues a buffer fill command - fills a device buffer with a single value
     * 
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param       count   counting how many elements will be filled in buffer
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param       offset  offset (in elements, not bytes) in buffer from which the fill will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer& buffer, 
        const T& value, const nr_uint& count, 
        const std::vector<Event>& wait, 
        const nr_uint& offset = 0, Event* notify = nullptr) const
    {
        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, wait.size(), wait.data(), (cl_event*) notify);
    }

    /**
     * @brief enqueues a buffer fill command - fills a device buffer with a single value
     * 
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param       count   counting how many elements will be filled in buffer
     * @param       offset  offset (in elements, not bytes) in buffer from which the fill will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer& buffer, 
        const T& value, const nr_uint& count, 
        const nr_uint& offset = 0, Event* notify = nullptr) const
    {
        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, 0, nullptr, (cl_event*) notify);
    }

    /**
     * @brief enqueues a buffer fill command. fills the entire buffer with a single value
     * 
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      internal OpenCL error status
     */
    template<typename T>
	cl_status enqueueBufferFillCommand(
        const Buffer& buffer, const T& value, Event* notify = nullptr) const
    {
		static_assert(
			sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16 || sizeof(T) == 32 || sizeof(T) == 64 || sizeof(T) == 128,
			"enqueueBufferFillCommand: value size has to be one of {1, 2, 4, 8, 16, 32, 64, 128}");

		cl_status err = CL_SUCCESS;
		auto s = buffer.size(&err);
		if (nr::error::isFailure(err))
		{
			return err;
		}
        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), size_t(0), s, size_t(0), nullptr, (cl_event*) notify);
    }
};

}
