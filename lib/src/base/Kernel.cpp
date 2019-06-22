#include <base/Kernel.h>

#include <base/Module.h>

namespace nr
{

Kernel::Kernel()
    : Wrapped()
{
}

Kernel::Kernel(const cl_kernel& kernel, const NRbool retain)
    : Wrapped(kernel, retain)
{
}

Kernel::Kernel(Module module, const string name, cl_status* err)
    : Kernel(clCreateKernel(module, name.c_str(), err))
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

}