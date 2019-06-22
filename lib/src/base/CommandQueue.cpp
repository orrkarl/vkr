#include <base/CommandQueue.h>

namespace nr
{

void CommandQueue::makeDefault(const CommandQueue& provided)
{
    defaultQueue = provided;
}

CommandQueue CommandQueue::getDefault()
{
    return defaultQueue;
}

CommandQueue::CommandQueue()
    : Wrapped()
{
}

CommandQueue::CommandQueue(const cl_command_queue& commandQueue, const NRbool retain = false)
    : Wrapped(CommandQueue, retain)
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

CommandQueue::CommandQueue(Context& context, Device& device, cl_command_queue_properties& properties, cl_status* err = nullptr)
    : Wrapped(clCreateCommandQueue(context, device, properties, err))
{
}

CommandQueue::CommandQueue(Device& device, cl_command_queue_properties& properties, cl_status* err = nullptr)
    : Wrapped(clCreateCommandQueue(Context::getDefault(), device, properties, err))
{
}

CommandQueue::CommandQueue(cl_command_queue_properties& properties, cl_status* err = nullptr)
    : Wrapped(clCreateCommandQueue(Context::getDefault(), Device::GetDefault(), properties, err))
{
}

CommandQueue& CommandQueue::operator=(const CommandQueue& other)
{
    return Wrapped::CommandQueue::operator=(other);
}

CommandQueue& CommandQueue::operator=(CommandQueue&& other)
{
    return Wrapped::CommandQueue::operator=(other);
}

CommandQueue::operator cl_command_queue() const 
{
    return object;
}    

cl_status CommandQueue::flush()
{
    return clFlush(object);
}
cl_status CommandQueue::await()
{
    return clFinish(object);
}
cl_status CommandQueue::finish()
{
    return clFinish(object);
}

}