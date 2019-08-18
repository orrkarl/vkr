#pragma once

#include "../predefs.h"

#include "../base/Buffer.h"
#include "../kernels/bin_rasterizer.cl.h"
#include "../rendering/Render.h"
#include "../utils/StandardDispatch.h"

#include <cmath>


namespace nr
{

namespace detail
{

class BinRasterizer : public StandardDispatch<2, Buffer, nr_uint, ScreenDimension, BinQueueConfig, Buffer, Buffer, Buffer>
{
public:
	BinRasterizer(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, BIN_RASTER_KERNEL_NAME, err)
	{
	}

	BinRasterizer()
		: StandardDispatch()
	{
	}

	cl_status setTriangleInputBuffer(const Buffer& buffer)
	{
		return setArg<TRIANGLE_BUFFER>(buffer);
	}

	cl_status setTriangleCount(const nr_uint count)
	{
		return setArg<TRIANGLE_COUNT>(count);
	}

	cl_status setScreenDimension(const ScreenDimension& screenDim)
	{
		return setArg<SCREEN_DIMENSION>(screenDim);
	}

	cl_status setBinQueueConfig(const BinQueueConfig& config)
	{
		return setArg<BIN_QUEUE_CONFIG>(config);
	}

	cl_status setOvereflowNotifier(const Buffer& overflow)
	{
		return setArg<OVERFLOW_BUFFER>(overflow);
	}

	cl_status setBinQueuesBuffer(const Buffer& binQueues)
	{
		return setArg<BIN_QUEUES_BUFFER>(binQueues);
	}

	cl_status setGlobalBatchIndex(const Buffer& batchIndex)
	{
		return setArg<BATCH_INDEX>(batchIndex);
	}

	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
	{
		nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
		nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
		return setExecutionRange(binCountX, binCountY, workGroupCount);
	}

	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
	{
		range.global.x = workGroupCount * binCountX;
		range.global.y = binCountY;

		range.local.x = binCountX;
		range.local.y = binCountY;
	}

private:
	static constexpr const nr_uint TRIANGLE_BUFFER = 0;
	static constexpr const nr_uint TRIANGLE_COUNT = 1;
	static constexpr const nr_uint SCREEN_DIMENSION = 2;
	static constexpr const nr_uint BIN_QUEUE_CONFIG = 3;
	static constexpr const nr_uint OVERFLOW_BUFFER = 4;
	static constexpr const nr_uint BIN_QUEUES_BUFFER = 5;
	static constexpr const nr_uint BATCH_INDEX = 6;
};


}

}