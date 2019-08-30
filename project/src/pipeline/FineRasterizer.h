/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief implements the fine rasterizer kernel
 * @version 0.6.0
 * @date 2019-08-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../base/Buffer.h"
#include "../kernels/fine_rasterizer.cl.h"
#include "../rendering/Render.h"
#include "../utils/StandardDispatch.h"

namespace nr
{

namespace detail
{

/**
 * @brief Accessing the fourth stage in the raster pipeline
 * 
 * The fine rasterizer is the fourth stage of the raster pipeline. It's job is to take the bin queues, process them and draw
 * the results to the framebuffer. As of right now, this can only paint triangles in red (255, 0, 0), although that's an easily changable
 * hardcoded value.
 * 
 * @par
 * The execution is done on a 2D grid, only this time each work group is assigned a different number of bins which are than
 * processed by its work items.
 * 
 * @note The order of triangles is preserved: if a triangle is earlier in the user input, it will be rendered earlier on the bins it affects
 */
class FineRasterizer : public StandardDispatch<2, Buffer, Buffer, ScreenDimension, BinQueueConfig, nr_uint, Buffer, Buffer>
{
public:
/**
	 * @brief Find fine raster kernel in a module and creates accessor to it
	 * 
	 * @param module kernel containing module
	 * @param[out] err internal OpenCL call status
	 */
	FineRasterizer(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, FINE_RASTER_KERNEL_NAME, err)
	{
	}

	/**
	 * @brief Constructs a null fine rasterizer
	 * 
	 */
	FineRasterizer()
		: StandardDispatch()
	{
	}

	/**
	 * @brief set the kernel input buffer
	 * 
	 * @param buffer triangle buffer
	 * @return internal OpenCL call status
	 */
	cl_status setTriangleInputBuffer(const Buffer& in)
	{
		return setArg<TRIANGLE_BUFFER>(in);
	}

	/**
	 * @brief sets the actual bin queues buffer
	 * 
	 * @param binQueues bin raster queues buffer
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueuesBuffer(const Buffer& queues)
	{
		return setArg<BIN_QUEUES_BUFFER>(queues);
	}

	/**
	 * @brief set the target screen dimensions
	 * 
	 * @param screenDim screen dimensions (in pixels)
	 * @return internal OpenCL call status
	 */
	cl_status setScreenDimensions(const ScreenDimension& screenDim)
	{
		return setArg<SCREEN_DIMENSION>(screenDim);
	}

	/**
	 * @brief set the kernel bin queue config
	 * 
	 * This argument sets the size of each bin and max element count per each bin queue
	 * @param config bin queue configuration
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueuesConfig(const BinQueueConfig& config)
	{
		return setArg<BIN_QUEUE_CONFIG>(config);
	}

	/**
	 * @brief Sets the expected work group amount used at bin rasterization
	 * 
	 * This argument should equal get_num_groups(0) at the bin rasterizer, and is critical for correct processing of the bin queues
	 * @param workGroupCount bin rasterizer work group count
	 * @return internal OpenCL call status
	 */
	cl_status setBinningWorkGroupCount(const nr_uint workGroupCount)
	{
		return setArg<WORK_GROUP_COUNT>(workGroupCount);
	}

	/**
	 * @brief sets the colorbuffer target
	 * 
	 * @param color target colorbuffer
	 * @return internal OpenCL call status
	 */
	cl_status setColorBuffer(const Buffer& color)
	{
		return setArg<COLOR_BUFFER>(color);
	}

	/**
	 * @brief sets the depthbuffer target
	 * 
	 * @param depth target colorbuffer
	 * @return internal OpenCL call status
	 */
	cl_status setDepthBuffer(const Buffer& depth)
	{
		return setArg<DEPTH_BUFFER>(depth);
	}

	/**
	 * @brief sets the framebuffer (both color and depth buffers)
	 * 
	 * @see setColorBuffer
	 * @see setDepthBuffer
	 * @param frame framebuffer
	 * @return internal OpenCL call status
	 */
	cl_status setFrameBuffer(const FrameBuffer& frame)
	{
		cl_status ret = setColorBuffer(frame.color);
		if (error::isFailure(ret)) return ret;

		return setDepthBuffer(frame.depth);
	}

	/**
	 * @brief calculates the execution range for this stage
	 * 
	 * @note this function will raise no erros; an invalid dimension will only raise errors when enqueueing to the command queue
	 * @see setExecutionRange(const nr_uint, const nr_uint, const nr_uint)
	 * 
	 * @param screenDim target screen size
	 * @param config bin queue configuration
	 * @param workGroupCount bin rasterizer work group count
	 */
	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
	{
		nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
		nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
		return setExecutionRange(binCountX, binCountY, workGroupCount);
	}

	/**
	 * @brief set the execution range for this stage
	 * 
	 * @note this function will raise no erros; an invalid dimension will only raise errors when enqueueing to the command queue
	 * 
	 * @param binCountX amount of bins on the X axis
	 * @param binCountY amount of bins on the Y axis
	 * @param workGroupCount bin rasterizer work group count
	 */
	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
	{
		range.global.x = binCountX;
		range.global.y = binCountY;

		range.local.x = binCountX;
		range.local.y = binCountY / workGroupCount;
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

}

}