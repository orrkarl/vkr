#pragma once

#include "../predefs.h"

#include <vector>

#include "Wrapper.h"

namespace nr::base {

class EventWaitException : public CLApiException {
public:
    explicit EventWaitException(Status status);
};

class BadEventExecutionStatus : public CLApiException {
public:
    explicit BadEventExecutionStatus(Status status);
};

class MemoryView : public ObjectView<cl_mem> {
public:
    explicit MemoryView(cl_mem memory);

    size_t sizeOnDevice() const;
};

/**
 * @brief Wrapper class for OpenCL cl_event - used for synchronization between commands, the device and the
 * host
 */
class EventView {
public:
    enum class ExecutionStatus : cl_int {
        Queued = CL_QUEUED,
        Submitted = CL_SUBMITTED,
        Running = CL_RUNNING,
        Completed = CL_COMPLETE
    };

    /**
     * @brief Blocks until all events in list reached CL_COMPLETE
     *
     * @note although it accepts a vector, passing more than MAX_UINT elements will throw
     *
     * @param events event list to wait
     * @return cl_status internal OpenCL call status
     */
    static void await(const std::vector<EventView>& events);

    explicit EventView(cl_event rawEvent);

    /**
     * @brief Blocks until this event reaches CL_COMPLETE
     * @note wraps clWaitForEvents
     * @return cl_status internal OpenCL call status
     */
    void await() const;

    ExecutionStatus status() const;

    cl_event rawHandle();

private:
    cl_event m_event;
};

using DeviceView = ObjectView<cl_device_id>;

}