#include "CommandQueue.h"

namespace nr::base {

CommandEnqueueException::CommandEnqueueException(Status status, const char* description)
    : CLApiException(status, description) {
}

CommandQueue::CommandQueue(Context& context, Device& device, PropertyBitField createProperties)
    : m_object(createCommandQueue(context, device, createProperties)) {
}

void CommandQueue::flush() const {
    auto status = clFlush(m_object);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not flush command queue");
    }
}

void CommandQueue::await() const {
    auto status = clFinish(m_object);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not flush command queue");
    }
}

void CommandQueue::finish() const {
    await();
}

ApiEvent CommandQueue::enqueueBufferReadCommand(const Buffer& buffer, size_t count, void* dest,
                                                const std::vector<Event>& waits, size_t offset /* = 0*/) {
    cl_event ret = cl_event();

    auto rawWaits = util::extractSizeLimited<Event, cl_event>(waits, std::mem_fn(&Event::rawHandle));
    auto status = clEnqueueReadBuffer(m_object, buffer.rawHandle(), CL_FALSE, offset, count, dest,
                                      rawWaits.size(), rawWaits.data(), &ret);
    if (status != CL_SUCCESS) {
        throw CommandEnqueueException(status, "cannot enqueue read command");
    }

    return ApiEvent(ret);
}

ApiEvent CommandQueue::enqueueBufferWriteCommand(Buffer& buffer, size_t count, const void* src,
                                                 const std::vector<Event>& waits, size_t offset /* = 0 */) {
    cl_event ret = cl_event();

    auto rawWaits = util::extractSizeLimited<Event, cl_event>(waits, std::mem_fn(&Event::rawHandle));
    auto status = clEnqueueWriteBuffer(m_object, buffer.rawHandle(), CL_FALSE, offset, count, src,
                                       static_cast<U32>(rawWaits.size()), rawWaits.data(), &ret);
    if (status != CL_SUCCESS) {
        throw CommandEnqueueException(status, "cannot enqueue write command");
    }

    return ApiEvent(ret);
}

cl_command_queue CommandQueue::createCommandQueue(Context& context, Device& device,
                                                  CommandQueue::PropertyBitField properties) {
    Status status = CL_SUCCESS;

    auto ret = clCreateCommandQueue(context.rawHandle(), device.rawHandle(),
                                    static_cast<cl_command_queue_properties>(properties), &status);
    if (status != CL_SUCCESS) {
        throw CommandQueueCreateException(status);
    }

    return ret;
}

}