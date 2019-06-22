#include <base/Context.h>

namespace nr
{

static void Context::makeDefault(const Context& provided)
{
    defaultContext = provided;
}

static Context Context::getDefault()
{
    return defaultContext;
}

Context::Context()
    : Wrapped()
{
}

Context::Context(const cl_context& context, const NRbool retain = false)
    : Wrapped(Context, retain)
{
}

Context::Context(const Context& other)
    : Wrapped(other)
{
}

Context::Context(Context&& other)
    : Wrapped(other)
{
}

Context::Context(
    const cl_context_properties* properties, 
    std::vector<Device>& devices, 
    cl_status* err = nullptr)
    : Wrapped(
        clCreateContext(
            properties, 
            devices.size(), static_cast<cl_device_id*>(&devices.front()), 
            nullptr, nullptr, 
            err))
{
}

Context::Context(
    const cl_context_properties* properties, 
    cl_device_type deviceType = CL_DEVICE_TYPE_GPU,          
    cl_status* err = nullptr)
    : Wrapped(
        clCreateContextFromType(
            properties, 
            deviceType,
            nullptr, nullptr, 
            err))
{
}

Context& Context::operator=(const Context& other)
{
    return Wrapped::operator=(other);
}

Context& Context::operator=(Context&& other)
{
    return Wrapped::operator=(other);
}

Context::operator cl_context() const 
{
    return object;
}

}