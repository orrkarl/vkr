#include <pipeline/FineRasterizer.h>

namespace nr
{

namespace __internal
{

cl_int FineRasterizerParams::load(cl::Kernel kernel)
{
    cl_int err = CL_SUCCESS;
    if ((err = kernel.setArg(0, simplexData.getBuffer())) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(1, dim)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(2, binQueueConfig)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(3, binQueues.getBuffer())) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(4, frameBuffer.color)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(5, frameBuffer.depth)) != CL_SUCCESS) return err;
    return kernel.setArg(6, frameBuffer.stencil);
}

}

}