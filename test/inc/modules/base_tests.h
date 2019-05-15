#pragma once

#include "../includes.h"

#include <base/Module.h>
#include <base/Kernel.h>
#include <rendering/Render.h>
#include <kernels/base.cl.h>

using namespace nr;
using namespace nr::__internal;

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

private:
    cl::Buffer result;
};

void screenFromNDCTestTemplate(Kernel<ScreenFromNDCParams> kernel, cl::CommandQueue queue, const NDCPosition& ndc, const ScreenDimension& dim, const ScreenPosition& expected)
{
    cl_int err;

    kernel.global = cl::NDRange(1);
    kernel.local  = cl::NDRange(1);
    
    kernel.params.ndcPosition = ndc;
    kernel.params.dimension   = dim;

    err = kernel.initParams(queue);
    ASSERT_EQ(CL_SUCCESS, err) << "Failed to init kernel args:\t" << utils::stringFromCLError(err);

    err = kernel.loadParams();
    ASSERT_EQ(CL_SUCCESS, err) << "Failed to load kernel args:\t" << utils::stringFromCLError(err);

    err = kernel.apply(queue);
    ASSERT_EQ(CL_SUCCESS, err) << "Could not add screen_from_ndc to command queue:\t" << utils::stringFromCLError(err);

    err = queue.finish();
    ASSERT_EQ(CL_SUCCESS, err) << "Could not finish commmand queue:\t" << utils::stringFromCLError(err);

    ScreenPosition result;
    err = kernel.params.getResult(&result);
    ASSERT_EQ(CL_SUCCESS, err) << "could not retrive result buffer:\t" << utils::stringFromCLError(err);
    ASSERT_EQ(expected, result) << "Screen from NDC didn't return correct values";
}


TEST(Base, Compilation)
{
    cl_int err = CL_SUCCESS;
    Module base(clcode::base, "-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    auto log = base.getBuildLog(&err);
    ASSERT_EQ(CL_SUCCESS, err);
    ASSERT_EQ("", log) << log;
}

TEST(Base, ScreenFromNDC)
{
     cl_int err = CL_SUCCESS; 
 
    cl::CommandQueue queue = cl::CommandQueue::getDefault();

    Module base(clcode::base, "-cl-std=CL2.0 -Werror -D RENDER_DIMENSION=3", &err);
    ASSERT_EQ(CL_SUCCESS, err) << "Failed to compile base module:\t" << utils::stringFromCLError(err);

    auto screen_from_ndc = base.makeKernel<ScreenFromNDCParams>("screen_from_ndc_kernel", &err);
    ASSERT_EQ(CL_SUCCESS, err) << "Could not create screen_from_ndc kernel:\t" << utils::stringFromCLError(err);

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1, -1}, ScreenDimension{100, 100}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0, 0}, ScreenDimension{100, 100}, ScreenPosition{49, 49});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{1, 1}, ScreenDimension{100, 100}, ScreenPosition{99, 99});

    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{-1, -1}, ScreenDimension{99, 99}, ScreenPosition{0, 0});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{0, 0}, ScreenDimension{99, 99}, ScreenPosition{49, 49});
    screenFromNDCTestTemplate(screen_from_ndc, queue, NDCPosition{1, 1}, ScreenDimension{99, 99}, ScreenPosition{98, 98});
}