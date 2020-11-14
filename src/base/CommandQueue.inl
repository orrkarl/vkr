#pragma once

#include "../predefs.h"

namespace nr::base {

template <typename T>
EventView CommandQueue::enqueueBufferFillCommand(Buffer& buffer, const T& value, size_t count,
                                                 std::vector<EventView>& waits, size_t offset) {
    static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8 || sizeof(T) == 16
                      || sizeof(T) == 32 || sizeof(T) == 64 || sizeof(T) == 128,
                  "enqueueBufferFillCommand: value size has to be one of {1, 2, 4, 8, 16, 32, 64, 128}");
    cl_event ret = cl_event();

    auto rawWaits = EventView::extractEventsSizeLimited(waits);
    auto status = clEnqueueFillBuffer(m_object, buffer.view().rawHandle(), &value, sizeof(T),
                                      sizeof(value) * offset, sizeof(value) * count,
                                      static_cast<U32>(rawWaits.size()), rawWaits.data(), &ret);
    if (status != CL_SUCCESS) {
        throw CommandEnqueueException(status, "could not enqueue buffer fill command");
    }

    return EventView(ret);
}

template <U32 Dim>
EventView CommandQueue::enqueueKernelCommand(KernelView kernel, const NDExecutionRange<Dim>& range,
                                             std::vector<EventView>& waits, const NDRange<Dim>& offset) {
    cl_event ret = cl_event();

    auto rawWaits = EventView::extractEventsSizeLimited(waits);
    auto status = clEnqueueNDRangeKernel(m_object, kernel.rawHandle(), Dim, range.global.data,
                                         range.local.data, offset.data, static_cast<U32>(rawWaits.size()),
                                         rawWaits.data(), &ret);
    if (status != CL_SUCCESS) {
        throw CommandEnqueueException(status, "could not enqueue kernel");
    }

    return EventView(ret);
}

}