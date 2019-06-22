#include <pipeline/BinRasterizer.h>

namespace nr
{

namespace __internal
{

cl_status BinRasterizer::load()
{
    cl_status err = CL_SUCCESS;
    if ((err = setArg(0, triangleData)) != CL_SUCCESS) return err;
    if ((err = setArg(1, triangleCount)) != CL_SUCCESS) return err;
    if ((err = setArg(2, dimension)) != CL_SUCCESS) return err;
    if ((err = setArg(3, binQueueConfig)) != CL_SUCCESS) return err;
    if ((err = setArg(4, hasOverflow)) != CL_SUCCESS) return err;
    if ((err = setArg(5, binQueues)) != CL_SUCCESS) return err;
    return setArg(6, batchIndex);
}

}

}