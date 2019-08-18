#include <base/Event.h>

namespace nr
{

Event::Event()
    : Wrapped()
{
}

Event::Event(const cl_event& event, const nr_bool retain)
    : Wrapped(event, retain)
{
}

Event::Event(Context context, cl_status* err)
    : Wrapped(clCreateUserEvent(context, err))
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
    Wrapped::operator=(other);
    return *this;
}

Event& Event::operator=(Event&& other)
{
    Wrapped::operator=(other);
    return *this;
}

Event::operator cl_event() const
{
    return object;
}

cl_status Event::await() const
{
    return clWaitForEvents(1, &object);
}

cl_status Event::await(const std::vector<Event>& events)
{
    return clWaitForEvents(cl_uint(events.size()), (const cl_event*) &events.front());
}

}