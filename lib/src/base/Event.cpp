#include <base/Event.h>

namespace nr
{

Event::Event()
    : Wrapped()
{
}

Event::Event(const cl_event& event, const NRbool retain = false)
    : Wrapped(event, retain)
{
}

Event::Event(const Event& other)
    : Wrapped(other)
{
}

Event::Event(Event&& other)
    : Wrapped(other)
{
}

Event& Event::operator=(const Event& other)
{
    return Wrapped::operator=(other);
}

Event& Event::operator=(Event&& other)
{
    return Wrapped::operator=(other);
}

Event::operator cl_event() const
{
    return object;
}

cl_status Event::await() const
{
    return clWaitForEvents(1, this);
}

cl_status Event::await(const std::vector<Event>& events)
{
    return clWaitForEvents(events.size(), (const cl_event*) &events.front());
}

}