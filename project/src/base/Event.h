/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style wrapper for OpenCL cl_event
 * @version 0.6.0
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <functional>
#include <vector>

#include "Context.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Wrapper class for OpenCL cl_event - used for synchronization between commands, the device and the host
 * 
 */
class NRAPI Event : public Wrapper<cl_event>
{
public:
    /**
     * @brief Construct a null event
     * 
     */
    Event();

    /**
	 * @brief Convertes a raw OpenCL event to the C++ wrapper
	 * 
	 * @param event object to own
	 * @param retain should the reference count for the object be incremented
	 */
    Event(const cl_event& event, const nr_bool retain, cl_status& status);

    /**
     * @brief Creates a new event, bound to a ceratin context
     * 
     * @note wraps clCreateUserEvent
     * @param context parent context for this event
     * @param[out] err internal OpenCL call status
     */
    Event(const Context& context, cl_status& err);

    Event(const Event& other);

    Event(Event&& other);

    Event& operator=(const Event& other);

    Event& operator=(Event&& other);

    /**
     * @brief provides access to the underlying OpenCL event
     * 
     * @return cl_event underlying OpenCL command queue
     */
    operator cl_event() const;

    /**
     * @brief Blocks until this event reaches CL_COMPLETE
     * @note wraps clWaitForEvents
     * @return cl_status internal OpenCL call status
     */
    cl_status await() const;

    /**
     * @brief Registeres a callback to this event
     * 
     * The callback will be called every time the event reaches a certain stage
     * @tparam T user data type
     * @param listenedStatus the stage this callback is registered to
     * @param callback the user callback
     * @param data user data
     * @return cl_status internal OpenCL call status
     */
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

    /**
     * @brief Blocks until all events in list reached CL_COMPLETE
     * 
     * @param events event list to wait
     * @return cl_status internal OpenCL call status 
     */
    static cl_status await(const std::vector<Event>& events);
};

}
