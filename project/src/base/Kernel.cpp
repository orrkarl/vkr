#include "Kernel.h"

#include "Module.h"

namespace nr
{

Kernel::Kernel()
    : Wrapped()
{
}

Kernel::Kernel(const cl_kernel& kernel, const nr_bool retain)
    : Wrapped(kernel, retain)
{
}

Kernel::Kernel(const Module& module, const char* name, cl_status& err)
	: Kernel(clCreateKernel(module, name, &err))
{
}


Kernel::Kernel(const Module& module, const string& name, cl_status& err)
    : Kernel(module, name.c_str(), err)
{
}


Kernel::Kernel(const Kernel& other)
    : Wrapped(other)
{
}

Kernel::Kernel(Kernel&& other)
    : Wrapped(other)
{
}

Kernel& Kernel::operator=(const Kernel& other)
{
    Wrapped::operator=(other);
    return *this;
}

Kernel& Kernel::operator=(Kernel&& other)
{
    Wrapped::operator=(other);
    return *this;
}

Kernel::operator cl_kernel() const 
{
    return object;
} 

cl_kernel Kernel::get() const
{
	return object;
}

}