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

    static cl_status await(const std::vector<Event>& events);
};

}
