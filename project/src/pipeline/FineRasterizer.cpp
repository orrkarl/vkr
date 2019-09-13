#include "FineRasterizer.h"

namespace nr
{

namespace detail
{

constexpr nr_uint TRIANGLE_BUFFER = 0;
constexpr nr_uint BIN_QUEUES_BUFFER = 1;
constexpr nr_uint SCREEN_DIMENSION = 2;
constexpr nr_uint BIN_QUEUE_CONFIG = 3;
constexpr nr_uint WORK_GROUP_COUNT = 4;
constexpr nr_uint COLOR_BUFFER = 5;
constexpr nr_uint DEPTH_BUFFER = 6;

FineRasterizer::FineRasterizer(const Module& module, cl_status& err)
	: StandardDispatch(module, FINE_RASTER_KERNEL_NAME, err)
{
}

FineRasterizer::FineRasterizer()
	: StandardDispatch()
{
}

cl_status FineRasterizer::setTriangleInputBuffer(const Buffer& in)
{
	return setArg<TRIANGLE_BUFFER>(in);
}

cl_status FineRasterizer::setBinQueuesBuffer(const Buffer& queues)
{
	return setArg<BIN_QUEUES_BUFFER>(queues);
}

cl_status FineRasterizer::setScreenDimensions(const ScreenDimension& screenDim)
{
	return setArg<SCREEN_DIMENSION>(screenDim);
}

cl_status FineRasterizer::setBinQueuesConfig(const BinQueueConfig& config)
{
	return setArg<BIN_QUEUE_CONFIG>(config);
}

cl_status FineRasterizer::setBinningWorkGroupCount(const nr_uint workGroupCount)
{
	return setArg<WORK_GROUP_COUNT>(workGroupCount);
}

cl_status FineRasterizer::setColorBuffer(const Buffer& color)
{
	return setArg<COLOR_BUFFER>(color);
}

cl_status FineRasterizer::setDepthBuffer(const Buffer& depth)
{
	return setArg<DEPTH_BUFFER>(depth);
}

cl_status FineRasterizer::setFrameBuffer(const FrameBuffer& frame)
{
	cl_status ret = setColorBuffer(frame.color);
	if (error::isFailure(ret)) return ret;

	return setDepthBuffer(frame.depth);
}

void FineRasterizer::setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
{
	nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
	nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
	return setExecutionRange(binCountX, binCountY, workGroupCount);
}

void FineRasterizer::setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
{
	range.global.x = binCountX;
	range.global.y = binCountY;

	range.local.x = binCountX;
	range.local.y = binCountY / workGroupCount;
}


}

}