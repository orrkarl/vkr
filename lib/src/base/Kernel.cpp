#include <base/Kernel.h>

namespace nr
{

Kernel::Kernel()
    : Wrapped()
{
}

Kernel::Kernel(const cl_kernel& kernel, const NRbool retain = false)
    : Wrapped(Kernel, retain)
{
}

Kernel(Module module, const string name, cl_status* err)
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
    return Wrapped::operator=(other);
}

Kernel& Kernel::operator=(Kernel&& other)
{
    return Wrapped::operator=(other);
}

Kernel::operator cl_kernel() const 
{
    return object;
} 

}