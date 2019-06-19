#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

/**
 * A wrapper for cl::Kernel which changes the way arguments are loaded and executed 
 * instead of calling a function, you first set the args up
 * 
 **/
template<class Params>
class NR_SHARED Kernel : public cl::Kernel
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

    // Add this kernel to the queue, start executing it
    cl_int apply(cl::CommandQueue queue)
    {
        return queue.enqueueNDRangeKernel(*this, offset, global, local, &requirements, &notifier);
    }

    // Set all of this kernel's arguments
    cl_int loadParams()
    {
        return params.load(*this);
    }

    // Initialize what ever argument needs initialziation (usually write a value to a buffer)
    cl_int initParams(cl::CommandQueue queue)
    {
        return params.init(queue);
    }

    // load a kernel
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