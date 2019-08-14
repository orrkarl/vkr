#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"

#include "../utils/StandardDispatch.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* FINE_RASTER_KERNEL_NAME;

class FineRasterizerKernel : public StandardDispatch<2, Buffer, Buffer, ScreenDimension, BinQueueConfig, nr_uint, Buffer, Buffer>
{
public:
	FineRasterizerKernel(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, FINE_RASTER_KERNEL_NAME, err)
	{
	}

	FineRasterizerKernel()
		: StandardDispatch()
	{
	}

	cl_status setTriangleInputBuffer(const Buffer& in)
	{
		return setArg<TRIANGLE_BUFFER>(in);
	}

	cl_status setBinQueuesBuffer(const Buffer& queues)
	{
		return setArg<BIN_QUEUES_BUFFER>(queues);
	}

	cl_status setScreenDimensions(const ScreenDimension& screenDim)
	{
		return setArg<SCREEN_DIMENSION>(screenDim);
	}

	cl_status setBinQueuesConfig(const BinQueueConfig& config)
	{
		return setArg<BIN_QUEUE_CONFIG>(config);
	}

	cl_status setBinningWorkGroupCount(const nr_uint workGroupCount)
	{
		return setArg<WORK_GROUP_COUNT>(workGroupCount);
	}

	cl_status setColorBuffer(const Buffer& color)
	{
		return setArg<COLOR_BUFFER>(color);
	}

	cl_status setDepthBuffer(const Buffer& depth)
	{
		return setArg<DEPTH_BUFFER>(depth);
	}

	cl_status setFrameBuffer(const FrameBuffer& frame)
	{
		cl_status ret = setColorBuffer(frame.color);
		if (error::isFailure(ret)) return ret;

		return setDepthBuffer(frame.depth);
	}

	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
	{
		nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
		nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
		return setExecutionRange(binCountX, binCountY, workGroupCount);
	}

	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
	{
		range.global.x = binCountX;
		range.global.y = binCountY;

		range.local.x = binCountX;
		range.local.y = binCountY;
	}

private:
	static constexpr const nr_uint TRIANGLE_BUFFER = 0;
	static constexpr const nr_uint BIN_QUEUES_BUFFER = 1;
	static constexpr const nr_uint SCREEN_DIMENSION = 2;
	static constexpr const nr_uint BIN_QUEUE_CONFIG = 3;
	static constexpr const nr_uint WORK_GROUP_COUNT = 4;
	static constexpr const nr_uint COLOR_BUFFER = 5;
	static constexpr const nr_uint DEPTH_BUFFER = 6;
};

namespace clcode
{

NRAPI extern const char* fine_rasterizer;

}

}

}