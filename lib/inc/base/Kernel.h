#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

template<class Params>
class NR_SHARED_EXPORT Kernel : public cl::Kernel
{

public:
    Kernel() {}

    explicit Kernel(cl::Kernel kernel)
        : cl::Kernel(kernel)
    {
    }

    Kernel(const cl::Program& code, const string& name, cl_int* err)
        : cl::Kernel(code, name.data(), err)
    {
    }

    cl_int apply(cl::CommandQueue queue)
    {
        return queue.enqueueNDRangeKernel(*this, offset, global, local, &requirements, &notifier);
    }

    cl_int loadParams()
    {
        return params.load(*this);
    }

    cl_int initParams(cl::CommandQueue queue)
    {
        return params.init(queue);
    }

    cl_int operator()(cl::CommandQueue queue)
    {
        cl_int err;
        if ((err = initParams(queue)) != CL_SUCCESS) return err;;
        if ((err = loadParams()) != CL_SUCCESS) return err;
        return apply(queue);
    }

public:
    cl::NDRange offset;
    cl::NDRange global;
    cl::NDRange local;
    std::vector<cl::Event> requirements;
    cl::Event notifier;
    Params params;
};

}

}