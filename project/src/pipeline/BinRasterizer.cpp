#include "BinRasterizer.h"

namespace nr
{

namespace detail
{

constexpr nr_uint TRIANGLE_BUFFER = 0;
constexpr nr_uint TRIANGLE_COUNT = 1;
constexpr nr_uint SCREEN_DIMENSION = 2;
constexpr nr_uint BIN_QUEUE_CONFIG = 3;
constexpr nr_uint OVERFLOW_BUFFER = 4;
constexpr nr_uint BIN_QUEUES_BUFFER = 5;
constexpr nr_uint BATCH_INDEX = 6;

BinRasterizer::BinRasterizer(const Module& module, cl_status& err)
	: StandardDispatch(module, BIN_RASTER_KERNEL_NAME, err)
{
}

BinRasterizer::BinRasterizer()
	: StandardDispatch()
{
}

cl_status BinRasterizer::setTriangleRecordInputBuffer(const Buffer& buffer)
{
	return setArg<TRIANGLE_BUFFER>(buffer);
}

cl_status BinRasterizer::setTriangleCount(const nr_uint count)
{
	return setArg<TRIANGLE_COUNT>(count);
}

cl_status BinRasterizer::setScreenDimension(const ScreenDimension& screenDim)
{
	return setArg<SCREEN_DIMENSION>(screenDim);
}

cl_status BinRasterizer::setBinQueueConfig(const BinQueueConfig& config)
{
	return setArg<BIN_QUEUE_CONFIG>(config);
}

cl_status BinRasterizer::setOvereflowNotifier(const Buffer& overflow)
{
	return setArg<OVERFLOW_BUFFER>(overflow);
}

cl_status BinRasterizer::setBinQueuesBuffer(const Buffer& binQueues)
{
	return setArg<BIN_QUEUES_BUFFER>(binQueues);
}

cl_status BinRasterizer::setGlobalBatchIndex(const Buffer& batchIndex)
{
	return setArg<BATCH_INDEX>(batchIndex);
}

void BinRasterizer::setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
{
	nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
	nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
	return setExecutionRange(binCountX, binCountY, workGroupCount);
}

void BinRasterizer::setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
{
	range.global.x = workGroupCount * binCountX;
	range.global.y = binCountY;

	range.local.x = binCountX;
	range.local.y = binCountY;
}

}

}