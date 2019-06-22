#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/h"

namespace nr
{

namespace __internal
{

struct NR_SHARED VertexShader : Kernel
{
    cl_status init(CommandQueue q) { return CL_SUCCESS; }

    cl_status load(Kernel kernel)
    {
        cl_status err = CL_SUCCESS;
        
        if ((err = setArg(0, points)) != CL_SUCCESS) return err;
        if ((err = setArg(1, near)) != CL_SUCCESS) return err;
        if ((err = setArg(2, far)) != CL_SUCCESS) return err;
        return setArg(3, result);
    }

    Buffer points;
    Buffer near;
    Buffer far;
    Buffer result;
};

}

}