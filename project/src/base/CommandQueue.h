/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief main interface for actions done on the OpenCL device
 * @version 0.6.0
 * @date 2019-08-26
 * @copyright Copyright (c) 2019
 */
#pragma once

#include "../predefs.h"

#include "Buffer.h"
#include "Context.h"
#include "Dispatch.h"
#include "Event.h"
#include "Kernel.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Execution size for a given Kernel
 * 
 * @tparam dim given execution dimension 
 */
template <nr_uint dim>
struct NDRange {};

/**
 * @brief Execution range of a 1-D Kernel
 * 
 * A 1-D Kernel executes on a single "line" of work items, and has just a "width" 
 * @tparam execution range 1 
 */
template <>
struct NDRange<1>
{
	union
	{
        /**
         * @brief execution range width 
         */
		nr_size x;
		nr_size data[1];
	};
};

/**
 * @brief Execution range of a 2-D kernel
 * 
 * A 2-D Kernel executes on a 2-D grid, meaning it has a "width" and a "height"
 * @tparam execution range 2
 */
template <>
struct NDRange<2>
{
	union
	{
		struct
		{
            /**
             * @brief execution range width
             */
			nr_size x;

            /**
             * @brief execution range height
             */
			nr_size y;
		};
		nr_size data[2];
	};
};

/**
 * @brief Execution range of a 3-D Kernel
 * 
 * A 3-D Kernel executes on a 3-D grid, meaning it has a "width", "height" and "depth"
 * @tparam execution range 3
 */
template <>
struct NDRange<3>
{
	union
	{
		struct
		{
            /**
             * @brief execution range width
             */
			nr_size x;

            /**
             * @brief execution range height
             */
			nr_size y;

            /**
             * @brief execution range depth 
             */
			nr_size z;
		};
		nr_size data[3];
	};
};

/**
 * @brief Typical data reuired to execute a kernel
 * 
 * @see template NDRange
 * @tparam dim execution dimension
 */
template <nr_uint dim>
struct NDExecutionRange
{
	NDRange<dim> global, local;
};

/**
 * @brief Wrapper class for OpenCL command queue, allowing for a more type safe and 'cpp-esque' interface.
 * 
 * Every importent interaction with the device has to go throw this class: kernel execution, buffer read\ write and other types of memory access
 * @note This class is written as a "lazy" wrapper: I will only implement functions if I intend on using them, which is why many functions of cl_command_queue aren't implemented
 */
class NRAPI CommandQueue : public Wrapper<cl_command_queue>
{
public:
    /**
     * @brief Construct a null command queue
     * 
     */
    CommandQueue();

    /**
	 * @brief Convertes a raw OpenCL command queue to the C++ wrapper
	 * 
     * This method allowes the class to "take ownership" of the lower OpenCL type; It may retain (increase the reference count) of the object
	 * @param commandQueue object to own
	 * @param retain should the reference count for the object be incremented
	 */
    CommandQueue(const cl_command_queue& commandQueue, const nr_bool retain, cl_status& status);

    /**
     * @brief Creates a bound and valid command queue; wraps clCreateCommandQueue
     * Refer to clCreateCommandQueue's documentation for a detailed explenation of the properties
     * @param context parent context of this queue
     * @param device execution device for this queue
     * @param properties command queue properties
     * @param[out] err function success status
     */
    CommandQueue(const Context& context, const Device& device, cl_command_queue_properties properties, cl_status& err);

    CommandQueue(const CommandQueue& other);

    CommandQueue(CommandQueue&& other);

    CommandQueue& operator=(const CommandQueue& other);

    CommandQueue& operator=(CommandQueue&& other);

    /**
     * @brief provides access to the underlying OpenCL command queue
     * 
     * @return cl_command_queue underlying OpenCL command queue
     */
    operator cl_command_queue() const;

    /**
     * @brief ensures that all of the waiting commands will be transfered to the device side queue
     * @note wraps clFlush
     * @note this function doesn't gurentee that all the command in the queue were executed
     * @return cl_status internal OpenCL call error status
     */
    cl_status flush() const;

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @see finish
     * @return cl_status internal OpenCL call error status
     */
    cl_status await() const;

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @note wraps clFinish
     * @return cl_status internal OpenCL call error status
     */
    cl_status finish() const;
    
    /**
     * @brief enqueues a buffer read command - moving data from a device buffer to host memory
     * @note wraps clEnqueueReadBuffer
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       block   should the function block (wait until the data read is completed)
     * @param       count   counting how many elements of T will be copied from the buffer
     * @param[out]  data    destination for the memory copy operation
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
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
     * @note wraps clEnqueueReadBuffer
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       block   should the function block (wait until the data read is completed)
     * @param       count   counting how many elements of T will be copied from the buffer
     * @param[out]  data    destination for the memory copy operation
     * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
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
	 * @note wraps clEnqueueReadBuffer
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param[out]  data    destination for the memory copy operation
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      cl_status internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferReadCommand(const Buffer& buffer, nr_bool block, T* data, Event* notify = nullptr) const
	{
        cl_status status;
        auto s = buffer.size(status);
        if (error::isFailure(status)) return status;
		return clEnqueueReadBuffer(
			object, buffer, block, 0, s, data,
			0, nullptr, (cl_event*)notify);
	}

	/**
	 * @brief enqueues a buffer write command - moving data from host memory to a device buffer
	 * @note wraps clEnqueueWriteBuffer
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param       count   counting how many elements of T will be copied from the buffer
	 * @param       data    source for memory copy operation
	 * @param       wait    list of events that have to complete before this command will begin execution
	 * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      cl_status internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferWriteCommand(const Buffer& buffer, nr_bool block, nr_uint count, const T* data, const std::vector<Event>& wait, nr_uint offset = 0, Event * notify = nullptr) const
	{
		return clEnqueueWriteBuffer(
			object, buffer, block, offset * sizeof(T), count * sizeof(T), data,
			wait.size(), (const cl_event*)wait.data(), (cl_event*)notify);
	}

	/**
	 * @brief enqueues a buffer write command - moving data from host memory to a device buffer
	 * @note wraps clEnqueueWriteBuffer
	 * @tparam      T       underlying buffer's element type
	 * @param       buffer  device buffer object
	 * @param       block   should the function block (wait until the data read is completed)
	 * @param       count   counting how many elements of T will be copied from the buffer
	 * @param       data    source for memory copy operation
	 * @param       offset  offset (in elements, not bytes) in buffer from which the copy will begin
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      cl_status internal OpenCL error status
	 */
	template<typename T>
	cl_status enqueueBufferWriteCommand(const Buffer& buffer, nr_bool block, nr_uint count, const T* data, nr_uint offset = 0, Event * notify = nullptr) const
	{
		return clEnqueueWriteBuffer(
			object, buffer, block, offset * sizeof(T), count * sizeof(T), data,
			0, nullptr, (cl_event*)notify);
	}

    /**
     * @brief enqueues a kernel command - submits a kernel to the device
     * @note wraps clEnqueueNDRangeKernel
     * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @tparam      KernelLike an object which can be casted to cl_kernel
     * @param       kernel  kernel to be submitted
     * @param       global  global execution size
     * @param       local   local execution size
     * @param       offset  local execution offset
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
     */
    template<nr_uint dim, typename KernelLike>
    cl_status enqueueKernelCommand(
        const KernelLike& kernel, 
        const NDRange<dim>& global, const NDRange<dim>& local,
        const NDRange<dim>& offset, const std::vector<Event>& wait, Event* notify = nullptr) const
    {
        static_assert(1 <= dim && dim <= 3, "execution dimension has to be between 1 and 3");
        return clEnqueueNDRangeKernel(object, static_cast<cl_kernel>(kernel), dim, offset.data, global.data, local.data, wait.size(), wait.data(), (cl_event*) notify);
    }

    /**
     * @brief enqueues a kernel command - submits a kernel to the device
     * @note wraps clEnqueueNDRangeKernel
     * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @tparam      KernelLike an object which can be casted to cl_kernel
     * @param       kernel  kernel to be submitted
     * @param       global  global execution size
     * @param       local   local execution size
     * @param       offset  local execution offset
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
     */
    template<nr_uint dim, typename KernelLike>
    cl_status enqueueKernelCommand(
        const KernelLike& kernel, 
        const NDRange<dim>& global, const NDRange<dim>& local,
        const NDRange<dim>& offset = NDRange<dim>{}, Event* notify = nullptr) const
    {
        static_assert(1 <= dim && dim <= 3, "execution dimension has to be between 1 and 3");
        return clEnqueueNDRangeKernel(object, static_cast<cl_kernel>(kernel), dim, offset.data, global.data, local.data, 0, nullptr, (cl_event*) notify);
    }


    /** 
     * @brief enqueues a kernel command - submits a kernel to the device
     * @note wraps clEnqueueNDRangeKernel
     * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @tparam      KernelLike an object which can be casted to cl_kernel
     * @param       kernel  kernel to be submitted
     * @param       range	spcification of the kernel execution range
     * @param       offset  local execution offset
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
    */
	template<nr_uint dim, typename KernelLike>
	cl_status enqueueKernelCommand(
		const KernelLike& kernel,
		const NDExecutionRange<dim>& range,
		const NDRange<dim> & offset, const std::vector<Event> & wait, Event * notify = nullptr) const
	{
        static_assert(1 <= dim && dim <= 3, "execution dimension has to be between 1 and 3");
		return clEnqueueNDRangeKernel(object, static_cast<cl_kernel>(kernel), dim, offset.data, range.global.data, range.local.data, wait.size(), wait.data(), (cl_event*)notify);
	}

	/**
	 * @brief enqueues a kernel command - submits a kernel to the device
     * @note wraps clEnqueueNDRangeKernel
	 * @tparam      dim     execution dimension - has to be between 1, 2 or 3
     * @tparam      KernelLike an object which can be casted to cl_kernel
	 * @param       kernel  kernel to be submitted
	 * @param       range	spcification of the kernel execution range
	 * @param       offset  local execution offset
	 * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
	 * @return      cl_status internal OpenCL error status
	 */
	template<nr_uint dim, typename KernelLike>
	cl_status enqueueKernelCommand(
		const KernelLike& kernel,
		const NDExecutionRange<dim>& range,
		const NDRange<dim> & offset = NDRange<dim>{}, Event * notify = nullptr) const
	{
        static_assert(1 <= dim && dim <= 3, "execution dimension has to be between 1 and 3");
		return clEnqueueNDRangeKernel(object, static_cast<cl_kernel>(kernel), dim, offset.data, range.global.data, range.local.data, 0, nullptr, (cl_event*)notify);
	}

	/**
	 * @brief allowing extension to the command queue while keeping the code structure
     * 
     * Allowes for a more customized enqueue, e.g to encapsulate some buffer updates and kernel executions under a single "command"
	 * @see Dispatch
	 * @tparam      T			CRTP dispatch type
	 * @param       dispatch	dispatch to execute
	 * @return      cl_status  internal	OpenCL error status
	 */
	template <typename T>
	cl_status enqueueDispatchCommand(const Dispatch<T>& dispatch) const
	{
		return dispatch(*this);
	}

    /**
     * @brief enqueues a buffer fill command - fills a device buffer with a single value
     * @note wraps clEnqueueFillBuffer
     * @note due to OpenCL standard limitations, the size of T has to be a power of 2 and smaller of equal to 128
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param       count   counting how many elements will be filled in buffer
     * @param       wait    list of events that have to complete before this command will begin execution
     * @param       offset  offset (in elements, not bytes) in buffer from which the fill will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer& buffer, 
        const T& value, const nr_uint& count, 
        const std::vector<Event>& wait, 
        const nr_uint& offset = 0, Event* notify = nullptr) const
    {
		static_assert(
			sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16 || sizeof(T) == 32 || sizeof(T) == 64 || sizeof(T) == 128,
			"enqueueBufferFillCommand: value size has to be one of {1, 2, 4, 8, 16, 32, 64, 128}");

        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, wait.size(), wait.data(), (cl_event*) notify);
    }

    /**
     * @brief enqueues a buffer fill command - fills a device buffer with a single value
     * @note wraps clEnqueueFillBuffer
     * @note due to OpenCL standard limitations, the size of T has to be a power of 2 and smaller of equal to 128
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param       count   counting how many elements will be filled in buffer
     * @param       offset  offset (in elements, not bytes) in buffer from which the fill will begin 
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
     */
    template<typename T>
    cl_status enqueueBufferFillCommand(
        const Buffer& buffer, 
        const T& value, const nr_uint& count, 
        const nr_uint& offset = 0, Event* notify = nullptr) const
    {
		static_assert(
			sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16 || sizeof(T) == 32 || sizeof(T) == 64 || sizeof(T) == 128,
			"enqueueBufferFillCommand: value size has to be one of {1, 2, 4, 8, 16, 32, 64, 128}");

        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), sizeof(value) * offset, sizeof(value) * count, 0, nullptr, (cl_event*) notify);
    }

    /**
     * @brief enqueues a buffer fill command. fills the entire buffer with a single value
     * @note wraps clEnqueueFillBuffer
     * @note due to OpenCL standard limitations, the size of T has to be a power of 2 and smaller of equal to 128
     * @tparam      T       underlying buffer's element type
     * @param       buffer  device buffer object
     * @param       value   element which will fill the buffer
     * @param[out]  notify  event which will be notified when this command changes status; will be ignored if nullptr
     * @return      cl_status internal OpenCL error status
     */
    template<typename T>
	cl_status enqueueBufferFillCommand(
        const Buffer& buffer, const T& value, Event* notify = nullptr) const
    {
		static_assert(
			sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16 || sizeof(T) == 32 || sizeof(T) == 64 || sizeof(T) == 128,
			"enqueueBufferFillCommand: value size has to be one of {1, 2, 4, 8, 16, 32, 64, 128}");

		cl_status err = CL_SUCCESS;
		auto s = buffer.size(err);
		if (nr::error::isFailure(err))
		{
			return err;
		}
        return clEnqueueFillBuffer(object, buffer, &value, sizeof(T), size_t(0), s, size_t(0), nullptr, (cl_event*) notify);
    }
};

}
