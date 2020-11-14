#include "Event.h"

namespace nr::base {

cl_event createUserEvent(Context& context) {
    Status status = CL_SUCCESS;

    auto ret = clCreateUserEvent(context.rawHandle(), &status);
    if (status != CL_SUCCESS) {
        throw EventCreateException(status);
    }

    return ret;
}

Event::Event(Context& context)
    : m_object(createUserEvent(context)) {
}

EventView Event::view() {
    return EventView(m_object);
}

}