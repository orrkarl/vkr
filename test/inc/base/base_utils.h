#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>
#include <rendering/Render.h>

using namespace nr;
using namespace testing;


struct NDCFromScreen : Kernel
{   
public:
    NDCFromScreen(Module module, cl_status* err = nullptr)
        : Kernel(module, "ndc_from_screen_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        if ((err = setArg(0, position)) != CL_SUCCESS) return err;
        if ((err = setArg(1, dimension)) != CL_SUCCESS) return err;        
        return setArg(2, result);
    }

    ScreenPosition position;
    ScreenDimension dimension;
    Buffer<NDCPosition> result;
};

struct ScreenFromNDC : Kernel
{   
public:
    ScreenFromNDC(Module module, cl_status* err = nullptr)
        : Kernel(module, "screen_from_ndc_test", err)
    {
    }

    cl_status load()
    {
        cl_status err = CL_SUCCESS;
        if ((err = setArg(0, ndcPosition)) != CL_SUCCESS) return err;
        if ((err = setArg(1, dimension)) != CL_SUCCESS) return err;        
        err = setArg(2, result);
        return err;
    }

    cl_status getResult(ScreenPosition* res) 
    {
        return defaultCommandQueue.enqueueBufferReadCommand(result, true, 1, res);
    } 

    NDCPosition ndcPosition;
    ScreenDimension dimension;
    Buffer<ScreenPosition> result;
};


AssertionResult comparePoints(const NDCPosition& p1, const NDCPosition& p2, const ScreenDimension& screenDim)
{
    nr_bool x = std::abs(p1.x - p2.x) <= 2.0f / screenDim.width;
    nr_bool y = std::abs(p1.y - p2.y) <= 2.0f / screenDim.height;

    if (x && y) return AssertionSuccess();
    
    if (x) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (X too far)";
    if (y) return AssertionFailure() << "Points" << p1 << ", " << p2 << " not in the same bin (y too far)";

    return AssertionFailure() << "Points " << p1 << ", " << p2 << " not in the same pixel (X and Y too far)";
}
