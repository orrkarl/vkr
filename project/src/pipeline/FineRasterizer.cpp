#include <pipeline/FineRasterizer.h>

namespace nr
{

namespace detail
{

cl_status FineRasterizer::load()
{
    cl_status err = CL_SUCCESS;
    if ((err = setArg(0, triangleData)) != CL_SUCCESS) return err;
    if ((err = setArg(1, binQueues)) != CL_SUCCESS) return err;
    if ((err = setArg(2, dim)) != CL_SUCCESS) return err;
    if ((err = setArg(3, binQueueConfig)) != CL_SUCCESS) return err;
    if ((err = setArg(4, workGroupCount)) != CL_SUCCESS) return err;
    if ((err = setArg(5, frameBuffer.color)) != CL_SUCCESS) return err;
    return setArg(6, frameBuffer.depth);
}

}

}