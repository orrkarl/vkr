#include <pipeline/BinRasterizer.h>

namespace nr
{

namespace __internal
{

const NRuint BinRasterizerParams::ZERO = 0;

cl_int BinRasterizerParams::load(cl::Kernel kernel)
{
    cl_int err = CL_SUCCESS;
    if ((err = kernel.setArg(0, m_simplexData)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(1, m_simplexCount)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(2, m_dim)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(3, m_binWidth)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(4, m_binHeight)) != CL_SUCCESS) return err;
    if ((err = kernel.setArg(5, m_binQueueSize)) != CL_SUCCESS) return err;
    return kernel.setArg(6, m_binQueues);
}

}

}