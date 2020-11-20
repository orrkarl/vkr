#include "Event.h"

namespace nr::base {

EventWaitException::EventWaitException(Status status)
    : CLApiException(status, "could not wait on event") {
}

BadEventExecutionStatus::BadEventExecutionStatus(Status status)
    : CLApiException(status, "the operation linked to this event failed") {
}

cl_event EventView::rawHandle() const {
    return m_rawEvent;
}

std::vector<cl_event> EventView::extractEventsSizeLimited(const std::vector<EventView>& events) {
    if (events.size() > std::numeric_limits<U32>::max()) {
        throw CLApiException(CL_INVALID_VALUE, "too many events");
    }

    std::vector<cl_event> rawEvents(events.size());
    std::transform(events.begin(), events.end(), rawEvents.begin(), std::mem_fn(&EventView::rawHandle));

    return rawEvents;
}

EventView::EventView(cl_event rawEvent)
    : m_rawEvent(rawEvent) {
}

Event::Event(cl_event rawEvent)
    : m_object(rawEvent) {
}

void Event::await(const std::vector<EventView>& events) {
    if (events.size() > std::numeric_limits<U32>::max()) {
        throw EventWaitException(CL_INVALID_VALUE);
    }

    auto rawEvents = EventView::extractEventsSizeLimited(events);
    auto status = clWaitForEvents(static_cast<U32>(events.size()), rawEvents.data());
    if (status != CL_SUCCESS) {
        throw EventWaitException(status);
    }
}

void Event::await() const {
    auto rawObject = m_object.underlying();
    auto status = clWaitForEvents(1, &rawObject);
    if (status != CL_SUCCESS) {
        throw EventWaitException(status);
    }
}

Event::ExecutionStatus Event::status() const {
    Status status = CL_SUCCESS;
    cl_int eventExecutionStatus = 0;

    status = clGetEventInfo(m_object, CL_EVENT_COMMAND_EXECUTION_STATUS, sizeof(eventExecutionStatus),
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

EventView Event::view() const {
    return EventView(m_object.underlying());
}

cl_event createUserEvent(Context& context) {
    Status status = CL_SUCCESS;

    auto ret = clCreateUserEvent(context.rawHandle(), &status);
    if (status != CL_SUCCESS) {
        throw EventCreateException(status);
    }

    return ret;
}

UserEvent::UserEvent(Context& context)
    : Event(createUserEvent(context)) {
}

ApiEvent::ApiEvent(cl_event rawEvent)
    : Event(rawEvent) {
}

}