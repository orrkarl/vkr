#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "../rendering/Render.h"

namespace nr
{

namespace __internal
{

struct NR_SHARED VertexShader : Kernel
{
    VertexShader(Module module, cl_status* err = nullptr)
        : Kernel(module, "shade_vertex", err)
    {
    }
    
    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        
        if ((err = setArg(0, points)) != CL_SUCCESS) return err;
        if ((err = setArg(1, near)) != CL_SUCCESS) return err;
        if ((err = setArg(2, far)) != CL_SUCCESS) return err;
        return setArg(3, result);
    }

    Buffer<NRfloat> points;
    Buffer<NRfloat> near;
    Buffer<NRfloat> far;
    Buffer<NRfloat> result;
};

}

}