#pragma once

#include "../predefs.h"

#include "BitField.h"
#include "Buffer.h"
#include "Context.h"
#include "Event.h"
#include "Kernel.h"
#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(CommandQueue);

class CommandEnqueueException : public CLApiException {
public:
    CommandEnqueueException(Status status, const char* description);
};

/**
 * @brief Execution size for a given Kernel
 *
 * @tparam Dim given execution dimension
 */
template <U32 Dim>
struct NDRange {
    static_assert(1 <= Dim && Dim <= 3, "invalid NDRange dimension");
};

/**
 * @brief Execution range of a 1-D Kernel
 *
 * A 1-D Kernel executes on a single "line" of work items, and has just a "width"
 * @tparam execution range 1
 */
template <>
struct NDRange<1> {
    union {
        /**
         * @brief execution range width
         */
        size_t x;
        size_t data[1];
    };
};

/**
 * @brief Execution range of a 2-D kernel
 *
 * A 2-D Kernel executes on a 2-D grid, meaning it has a "width" and a "height"
 * @tparam execution range 2
 */
template <>
struct NDRange<2> {
    union {
        struct {
            /**
             * @brief execution range width
             */
            size_t x;

            /**
             * @brief execution range height
             */
            size_t y;
        };
        size_t data[2];
    };
};

/**
 * @brief Execution range of a 3-D Kernel
 *
 * A 3-D Kernel executes on a 3-D grid, meaning it has a "width", "height" and "depth"
 * @tparam execution range 3
 */
template <>
struct NDRange<3> {
    union {
        struct {
            /**
             * @brief execution range width
             */
            size_t x;

            /**
             * @brief execution range height
             */
            size_t y;

            /**
             * @brief execution range depth
             */
            size_t z;
        };
        size_t data[3];
    };
};

/**
 * @brief Typical data required to execute a kernel
 *
 * @see template NDRange
 * @tparam Dim execution dimension
 */
template <U32 Dim>
struct NDExecutionRange {
    NDRange<Dim> global, local;
};

/**
 * @brief Wrapper class for OpenCL command queue, allowing for a more type safe and 'CPP-esque' interface.
 *
 * Every important interaction with the device has to go throw this class: kernel execution, buffer read\
 * write and other types of memory access
 */
class CommandQueue {
public:
    enum class PropertyFlag : cl_command_queue_properties {
        Default = 0,
        OutOfOrderExec = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
        ProfilingEnabled = CL_QUEUE_PROFILING_ENABLE
    };

    using Properties = EnumBitField<PropertyFlag>;

    /**
     * @brief Creates a bound and valid command queue; wraps clCreateCommandQueue
     * Refer to clCreateCommandQueue's documentation for a detailed explanation of the properties
     * @param context parent context of this queue
     * @param device execution device for this queue
     * @param properties command queue properties
     */
    CommandQueue(Context& context, DeviceView device, Properties createProperties);

    /**
     * @brief ensures that all of the waiting commands will be transferred to the device side queue
     * @note wraps clFlush
     * @note this function doesn't guarantee that all the command in the queue were executed
     */
    void flush() const;

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @see finish
     */
    void await() const;

    /**
     * @brief waits for the command queue to be empty - no commands wait for execution
     * @note wraps clFinish
     */
    void finish() const;

    /**
     * @brief enqueues a buffer read command - moving data from a device buffer to host memory
     *
     * This function doesn't block, but return an event which can be waited for finish
     *
     * @note wraps clEnqueueReadBuffer
     *
     * @param       buffer view of memory object to read from
     * @param       count  count of bytes to read from memory object
     * @param[out]  dest   local buffer to write the results into
     * @param       waits  list of events which have to complete before this command starts
     * @param       offset offset in memory object to read from
     *
     * @return event notifying this command's status
     */
    [[nodiscard]] ApiEvent enqueueBufferReadCommand(Buffer& buffer, size_t count, void* dest,
                                                    const std::vector<EventView>& waits, size_t offset = 0);

    /**
     * @brief enqueues a buffer write command - moving data from host memory to a device buffer
     *
     * This function doesn't block, but return an event which can be waited for finish
     *
     * @note wraps clEnqueueWriteBuffer
     *
     * @param  buffer view of memory object to read from
     * @param  count  count of bytes to read from memory object
     * @param  src    host source for memory copy operation
     * @param  waits  list of events that have to complete before this command will begin execution
     * @param  offset offset in memory object to read from
     *
     * @return event notifying this command's status
     */
    [[nodiscard]] ApiEvent enqueueBufferWriteCommand(Buffer& buffer, size_t count, const void* src,
                                                     const std::vector<EventView>& waits, size_t offset = 0);

    /**
     * @brief enqueues a buffer fill command - fills a device buffer with a single value
     *
     * @note wraps clEnqueueFillBuffer
     * @note due to OpenCL standard limitations, the size of T has to be a power of 2 and smaller or equal to
     * 128
     *
     * @tparam T       underlying buffer's element type
     * @param  buffer  device buffer object
     * @param  value   element which will fill the buffer
     * @param  count   counting how many elements will be filled in buffer
     * @param  waits   list of events that have to complete before this command will begin execution
     * @param  offset  offset (in elements, not bytes) in buffer from which the fill will begin
     *
     * @return event notifying this command's status
     */
    template <typename T>
    [[nodiscard]] ApiEvent enqueueBufferFillCommand(Buffer& buffer, const T& value, size_t count,
                                                    const std::vector<EventView>& waits, size_t offset = 0);

    /**
     * @brief enqueues a kernel command - submits a kernel to the device
     *
     * This function doesn't block, but return an event which can be waited for finish
     *
     * @note wraps clEnqueueNDRangeKernel
     *
     * @tparam Dim     execution dimension - has to be 1, 2 or 3
     * @param  kernel  kernel to be submitted
     * @praam  range   local and global execution ranges
     * @param  waits   list of events that have to complete before this command will begin execution
     * @param  offset  local execution offset
     *
     * @return event notifying this command's status
     */
    template <U32 Dim>
    [[nodiscard]] ApiEvent enqueueKernelCommand(const Kernel& kernel, const NDExecutionRange<Dim>& range,
                                                const std::vector<EventView>& waits,
                                                const NDRange<Dim>& offset);

private:
    struct CommandQueueTraits {
        using Type = cl_command_queue;

        static constexpr auto release = clReleaseCommandQueue;
    };

    UniqueWrapper<CommandQueueTraits> m_object;
};

cl_kernel Kernel::rawHandle() const {
    return m_object.underlying();
}

}

#include "CommandQueue.inl"