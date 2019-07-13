#include <pipeline/BinRasterizer.h>

#include <cmath>

namespace nr
{

namespace __internal
{

nr_uint BinRasterizer::getTotalBinQueueCount(const nr_uint workGroupCount, const ScreenDimension& dim, const BinQueueConfig config)
{
    return workGroupCount * getBinCount(dim, config.binWidth, config.binHeight) * (config.queueSize + 1) * sizeof(nr_uint);
}

nr_uint BinRasterizer::getBinCount(const ScreenDimension& dim, const nr_uint& binWidth, const nr_uint& binHeight)
{
    nr_uint xCount = (nr_uint) ceil(((nr_float) dim.width) / binWidth);
    nr_uint yCount = (nr_uint) ceil(((nr_float) dim.height) / binHeight);
    return xCount * yCount;
}

BinRasterizer::BinRasterizer(Module module, cl_status* err)
	: Kernel(module, "bin_rasterize", err), dimension{ 0 }, binQueueConfig{ 0, 0, 0 }, triangleCount{ 0 }
{
}

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