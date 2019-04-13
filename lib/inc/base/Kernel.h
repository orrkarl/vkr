#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

template<class Params>
class NR_SHARED_EXPORT Kernel
{

public:
    Kernel() {}

    explicit Kernel(cl::Kernel kernel)
        : m_kernel(kernel)
    {
    }

    Kernel(const cl::Program& code, const string& name, cl_int* err)
        : m_kernel(code, name.data(), err)
    {
    }

    cl_int apply(cl::CommandQueue queue)
    {
        return queue.enqueueNDRangeKernel(m_kernel, offset, global, local, &requirements, &notifier);
    }

    bool isValid()
    {
        cl_int err;
        auto ref_count = m_kernel.getInfo<CL_KERNEL_REFERENCE_COUNT>(&err);
        return err == CL_SUCCESS && ref_count > 0;
    }

    cl_int loadParams()
    {
        return Params::load(m_kernel, params);
    }

public:
    cl::NDRange offset;
    cl::NDRange global;
    cl::NDRange local;
    std::vector<cl::Event> requirements;
    cl::Event notifier;
    Params params;

protected:
    cl::Kernel m_kernel;
};

}

}