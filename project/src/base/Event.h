#pragma once

#include "../predefs.h"

#include <functional>
#include <vector>

#include "Context.h"
#include "Views.h"
#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(Event);

class Event {
public:
    /**
     * @brief Creates a new event, bound to a certain context
     *
     * @note wraps clCreateUserEvent
     * @param context parent context for this event
     */
    explicit Event(Context& context);

    EventView view();

private:
    struct EventTraits {
        static constexpr auto release = clReleaseEvent;
        using Type = cl_event;
    };

    UniqueWrapper<EventTraits> m_object;
};

}
