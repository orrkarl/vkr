#include "Kernel.h"

#include "Module.h"

namespace nr::base {

cl_kernel createKernel(cl_program program, const char* name)
{
    Status status = CL_SUCCESS;

    auto ret = clCreateKernel(program, name, &status);
    if (status != CL_SUCCESS) {
        throw KernelCreateException(status);
    }

    return ret;
}

Kernel::Kernel(Module& module, const char* name)
    : m_object(createKernel(module.rawHandle(), name))
{
}

Kernel::Kernel(Module& module, const std::string& name)
    : Kernel(module, name.c_str())
{
}

cl_kernel base::Kernel::rawHandle() const { return m_object; }

}