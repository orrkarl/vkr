#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>
#include <rendering/Render.h>
#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::__internal;
using namespace testing;


struct NDCFromScreenParams
{   
public:

    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        if ((err = kernel.setArg(0, position)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(1, dimension)) != CL_SUCCESS) return err;        
        return kernel.setArg(2, result);
    }

    cl_int init(cl::CommandQueue queue)
    {
        return CL_SUCCESS;
    }

    ScreenPosition position;
    ScreenDimension dimension;
    Buffer result;
};

typedef Kernel<NDCFromScreenParams> NDCFromScreen;

struct ScreenFromNDCParams
{   
public:
    ScreenFromNDCParams(cl_int* err = nullptr)
    {
        result = cl::Buffer(CL_MEM_WRITE_ONLY, sizeof(ScreenPosition), nullptr, err);
    }

    cl_int load(cl::Kernel kernel)
    {
        cl_int err = CL_SUCCESS;
        if ((err = kernel.setArg(0, ndcPosition)) != CL_SUCCESS) return err;
        if ((err = kernel.setArg(1, dimension)) != CL_SUCCESS) return err;        
        err = kernel.setArg(2, result);
        return err;
    }

    cl_int getResult(ScreenPosition* res) 
    {
        return cl::CommandQueue::getDefault().enqueueReadBuffer(result, CL_TRUE, 0, sizeof(ScreenPosition), res);
    } 

    cl_int init(cl::CommandQueue queue)
    {
        return CL_SUCCESS;
    }

    NDCPosition ndcPosition;
    ScreenDimension dimension;
    cl::Buffer result;
};


typedef Kernel<ScreenFromNDCParams> ScreenFromNDC;


AssertionResult comparePoints(const NDCPosition& p1, const NDCPosition& p2, const ScreenDimension& screenDim)
{
    NRbool x = std::abs(p1.x - p2.x) <= 2.0f / screenDim.width;
    NRbool y = std::abs(p1.y - p2.y) <= 2.0f / screenDim.height;

    if (x && y) return AssertionSuccess();
    
    if (x) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (X too far)";
    if (y) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (y too far)";

    return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (X and Y too far)";
}
