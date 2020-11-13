#include "Event.h"

namespace nr::base {

EventWaitException::EventWaitException(Status status)
    : CLApiException(status, "could not wait for event")
{
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

cl_event createUserEvent(Context& context)
{
    Status status = CL_SUCCESS;

    auto ret = clCreateUserEvent(context.rawHandle(), &status);
    if (status != CL_SUCCESS) {
        throw EventCreateException(status);
    }

    return ret;
}

Event::Event(Context& context)
    : m_object(createUserEvent(context))
{
}

EventView Event::view() { return EventView(m_object); }

}