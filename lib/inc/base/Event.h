#pragma once

#include "../general/predefs.h"

#include <vector>

#include "Wrapper.h"

namespace nr
{

class NR_SHARED Event : public Wrapper<cl_event>
{
// Functions and Constructors
public:
    Event();

    explicit Event(const cl_event& event, const NRbool retain = false);

    Event(const Event& other);

    Event(Event&& other);

    Event& operator=(const Event& other);

    Event& operator=(Event&& other);

    operator cl_event() const;

    cl_status await() const;

    template<typename T>
    cl_status registerCallback(
        cl_int listenedStatus, 
        const std::function<void(Event, cl_int, T* data)> callback, T* data)
    {
        return clSetEventCallback(
            object, listenedStatus, 
            static_cast<void(CL_CALLBACK *)(cl_event, cl_int, void*>(
                [callback](cl_event event, cl_int status, void* data){ callback(Event(event, true), status, (T*) data); }), 
            data);
    }

    static cl_status await(const std::vector<Event>& events);
};

}
