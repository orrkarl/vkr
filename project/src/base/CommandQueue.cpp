#include "CommandQueue.h"

namespace nr
{
CommandQueue::CommandQueue()
    : Wrapped()
{
}

CommandQueue::CommandQueue(const cl_command_queue& commandQueue, const nr_bool retain)
    : Wrapped(commandQueue, retain)
{
}

CommandQueue::CommandQueue(const CommandQueue& other)
    : Wrapped(other)
{
}

CommandQueue::CommandQueue(CommandQueue&& other)
    : Wrapped(other)
{
}

CommandQueue::CommandQueue(const Context& context, const Device& device, cl_command_queue_properties properties, cl_status& err)
    : Wrapped(clCreateCommandQueue(context, device, properties, &err))
{
}

CommandQueue& CommandQueue::operator=(const CommandQueue& other)
{
    Wrapped::operator=(other);
    return *this;
}

CommandQueue& CommandQueue::operator=(CommandQueue&& other)
{
    Wrapped::operator=(other);
    return *this;
}

CommandQueue::operator cl_command_queue() const 
{
    return object;
}    

cl_status CommandQueue::flush() const
{
    return clFlush(object);
}

cl_status CommandQueue::await() const
{
    return clFinish(object);
}

cl_status CommandQueue::finish() const
{
    return clFinish(object);
}

}