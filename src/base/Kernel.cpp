#include "Kernel.h"

#include "Module.h"

namespace nr::base {

cl_kernel createKernel(cl_program program, const char* name) {
    Status status = CL_SUCCESS;

    auto ret = clCreateKernel(program, name, &status);
    if (status != CL_SUCCESS) {
        throw KernelCreateException(status);
    }

    return ret;
}

Kernel::Kernel(Module& module, const char* name)
    : m_object(createKernel(module.rawHandle(), name)) {
}

Kernel::Kernel(Module& module, const std::string& name)
    : Kernel(module, name.c_str()) {
}

void Kernel::setArg(U32 index, size_t size, const void* value) {
    auto status = clSetKernelArg(m_object, index, size, value);
    if (status != CL_SUCCESS) {
        throw CLApiException(status, "could not set kernel argument");
    }
}

void Kernel::setArg(U32 index, Memory& value) {
    cl_mem memoryObj = value.rawHandle();
    return setArg(index, sizeof(memoryObj), &memoryObj);
}

cl_kernel Kernel::rawHandle() const {
    return m_object.underlying();
}

}