#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/Kernel.h"

namespace nr
{

namespace __internal
{

class VertexShadingParams
{
public:

    cl_int init(cl::CommandQueue q) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        NRuint argc = 0;

        if ((err = kernel.setArg(argc++, points.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, near.getBuffer())) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(argc++, far.getBuffer())) != CL_SUCCESS) return err;
        return kernel.setArg(argc, result.getBuffer());
    }

    Buffer points;
    Buffer near;
    Buffer far;
    Buffer result;
};

typedef Kernel<VertexShadingParams> VertexShader;

}

}