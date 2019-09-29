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
 * @brief Accessing the third stage in the raster pipeline
 * 
 * The fine rasterizer is the third stage of the raster pipeline. It's job is to take the bin queues, process them and draw
 * the results to the framebuffer. As of right now, this can only paint triangles in red (255, 0, 0), although that's an easily changable
 * hardcoded value.
 * 
 * @par
 * The execution is done on a 2D grid, only this time each work group is assigned a different number of bins which are than
 * processed by its work items.
 * 
 * @note The order of triangles is preserved: if a triangle is earlier in the user input, it will be rendered earlier on the bins it affects
 */
class NRAPI FineRasterizer : public StandardDispatch<2, Buffer, Buffer, ScreenDimension, BinQueueConfig, nr_uint, Buffer, Buffer>
{
public:
	/**
	 * @brief Find fine raster kernel in a module and creates accessor to it
	 * 
	 * @param module kernel containing module
	 * @param[out] err internal OpenCL call status
	 */
	FineRasterizer(const Module& module, cl_status& err);

	/**
	 * @brief Constructs a null fine rasterizer
	 * 
	 */
	FineRasterizer();

	/**
	 * @brief set the kernel input buffer
	 * 
	 * @param buffer triangle buffer
	 * @return internal OpenCL call status
	 */
	cl_status setTriangleRecordInputBuffer(const Buffer& in);

	/**
	 * @brief sets the actual bin queues buffer
	 * 
	 * @param binQueues bin raster queues buffer
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueuesBuffer(const Buffer& queues);

	/**
	 * @brief set the target screen dimensions
	 * 
	 * @param screenDim screen dimensions (in pixels)
	 * @return internal OpenCL call status
	 */
	cl_status setScreenDimensions(const ScreenDimension& screenDim);

	/**
	 * @brief set the kernel bin queue config
	 * 
	 * This argument sets the size of each bin and max element count per each bin queue
	 * @param config bin queue configuration
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueuesConfig(const BinQueueConfig& config);

	/**
	 * @brief Sets the expected work group amount used at bin rasterization
	 * 
	 * This argument should equal get_num_groups(0) at the bin rasterizer, and is critical for correct processing of the bin queues
	 * @param workGroupCount bin rasterizer work group count
	 * @return internal OpenCL call status
	 */
	cl_status setBinningWorkGroupCount(const nr_uint workGroupCount);

	/**
	 * @brief sets the colorbuffer target
	 * 
	 * @param color target colorbuffer
	 * @return internal OpenCL call status
	 */
	cl_status setColorBuffer(const Buffer& color);

	/**
	 * @brief sets the depthbuffer target
	 * 
	 * @param depth target colorbuffer
	 * @return internal OpenCL call status
	 */
	cl_status setDepthBuffer(const Buffer& depth);

	/**
	 * @brief sets the framebuffer (both color and depth buffers)
	 * 
	 * @see setColorBuffer
	 * @see setDepthBuffer
	 * @param frame framebuffer
	 * @return internal OpenCL call status
	 */
	cl_status setFrameBuffer(const FrameBuffer& frame);

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
	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount);

	/**
	 * @brief set the execution range for this stage
	 * 
	 * @note this function will raise no erros; an invalid dimension will only raise errors when enqueueing to the command queue
	 * 
	 * @param binCountX amount of bins on the X axis
	 * @param binCountY amount of bins on the Y axis
	 * @param workGroupCount bin rasterizer work group count
	 */
	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount);
};

}

}