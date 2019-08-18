/**
 * @file Event.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping cl_event
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

#include <functional>
#include <vector>

#include "Context.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Wrapper class for OpenCL cl_event - used to notify the host and device for commands status
 * 
 */
class NRAPI Event : public Wrapper<cl_event>
{
public:
    Event();

    explicit Event(const cl_event& event, const nr_bool retain = false);

    explicit Event(Context context, cl_status* err = nullptr);

    Event(const Event& other);

    Event(Event&& other);

    Event& operator=(const Event& other);

    Event& operator=(Event&& other);

    operator cl_event() const;

    cl_status await() const;

    template<typename T>
    cl_status registerCallback(
        cl_int listenedStatus, 
        const std::function<void(Event, cl_int, T*)> callback, T* data)
    {
        return clSetEventCallback(
            object, listenedStatus, 
            static_cast<void(CL_CALLBACK *)(cl_event, cl_int, void*)>(
                [callback](cl_event event, cl_int status, void* data){ callback(Event(event, true), status, (T*) data); }), 
            data);
    }

    static cl_status await(const std::vector<Event>& events);
};

}
