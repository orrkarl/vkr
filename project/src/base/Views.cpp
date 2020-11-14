#include "Views.h"

#include <algorithm>
#include <functional>
#include <limits>

namespace nr::base {

EventWaitException::EventWaitException(Status status)
    : CLApiException(status, "could not wait for event")
{
}

BadEventExecutionStatus::BadEventExecutionStatus(Status status)
    : CLApiException(status, "event waited action terminated abnormally")
{
}

MemoryView::MemoryView(cl_mem memory)
    : ObjectView(memory)
{
}

size_t MemoryView::sizeOnDevice() const
{
    size_t ret;

    auto status = clGetMemObjectInfo(m_rawObject, CL_MEM_SIZE, sizeof(ret), &ret, nullptr);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not acquire buffer size");
    }

    return ret;
}

std::vector<cl_event> EventView::extractEventsSizeLimited(std::vector<EventView>& events)
{
    if (events.size() > std::numeric_limits<U32>::max()) {
        throw CLApiException(CL_INVALID_VALUE, "too many events");
    }

    std::vector<cl_event> rawEvents(events.size());
    std::transform(events.begin(), events.end(), rawEvents.begin(), std::mem_fn(&EventView::rawHandle));

    return rawEvents;
}

void EventView::await(const std::vector<EventView>& events)
{
    if (events.size() > std::numeric_limits<U32>::max()) {
        throw EventWaitException(CL_INVALID_VALUE);
    }

    std::vector<cl_event> rawEvents(events.size());
    std::transform(
        events.cbegin(), events.cend(), rawEvents.begin(), [](EventView event) { return event.m_event; });

    auto status = clWaitForEvents(static_cast<U32>(events.size()), rawEvents.data());
    if (status != CL_SUCCESS) {
        throw EventWaitException(status);
    }
}

EventView::EventView(cl_event rawEvent)
    : m_event(rawEvent)
{
}

void EventView::await() const
{
    auto status = clWaitForEvents(1, &m_event);
    if (status != CL_SUCCESS) {
        throw EventWaitException(status);
    }
}

EventView::ExecutionStatus EventView::status() const
{
    Status status = CL_SUCCESS;
    cl_int eventExecutionStatus = 0;

    status = clGetEventInfo(m_event, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(eventExecutionStatus),
        &eventExecutionStatus, nullptr);

    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not query event execution status");
    }

    switch (eventExecutionStatus) {
    case CL_QUEUED:
        return ExecutionStatus::Queued;
    case CL_SUBMITTED:
        return ExecutionStatus::Submitted;
    case CL_RUNNING:
        return ExecutionStatus::Running;
    case CL_COMPLETE:
        return ExecutionStatus::Completed;
    default:
        throw BadEventExecutionStatus(eventExecutionStatus);
    }
}

cl_event EventView::rawHandle() { return m_event; }

}
