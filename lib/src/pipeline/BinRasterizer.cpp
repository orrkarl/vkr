#include <pipeline/BinRasterizer.h>

namespace nr
{

namespace __internal
{

cl_int BinRasterizerParams::load(cl::Kernel kernel)
{
    cl_int err = CL_SUCCESS;
    if ((err = kernel.setArg(0, triangleData.getBuffer())) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(1, triangleCount)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(2, dimension)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(3, binQueueConfig)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(4, hasOverflow.getBuffer())) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(5, binQueues.getBuffer())) != CL_SUCCESS) return err;
    return kernel.setArg(6, batchIndex.getBuffer());
}

}

}