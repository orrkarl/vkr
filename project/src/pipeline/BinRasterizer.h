/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief implements the bin rasterizer kernel
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
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

/**
 * @brief Accessing the second stage in the raster pipeline
 * 
 * The bin rasterizer is the second stage in the raster pipeline. It's job is to divide the triangles to bins. A bin is just a rectangle
 * of pixels of the screen of known width and height. By dividing the triangles to queues we can run a later fine rasterizer on a per bin basis.
 * 
 * @par
 * This stage divides the triangles into bin queues: queues of known size, where each queue is assigned to a specific bin. Each queue contains indices
 * to the triangle buffer. Since the exect load on each queue cannot be pre-determinated, each queue has to mark it's own ending.
 * If the queue is empty, its first element (which is the is_empty flag) will be set. Otherwise it will be zero and the queue 
 * will have data in it. After the last triangle index is assigned (and if there is still room in the queue), the queue will be declared ended with
 * 0. 
 * 
 * @par
 * The execution is done on a 2D grid (with each work item processing a bin), where each work group gets it's own colleciton of bin queues.
 * 
 * @note The order of triangles in every bin queue is preserved (triangles earlier in the user input will come earlier in the queue)
 */
class NRAPI BinRasterizer : public StandardDispatch<2, Buffer, nr_uint, ScreenDimension, BinQueueConfig, Buffer, Buffer, Buffer>
{
public:
	/**
	 * @brief Find bin raster kernel in a module and creates accessor to it
	 * 
	 * @param module kernel containing module
	 * @param[out] err internal OpenCL call status
	 */
	BinRasterizer(const Module& module, cl_status& err);

	/**
	 * @brief Constructs a null bin rasterizer
	 * 
	 */
	BinRasterizer();

	/**
	 * @brief set the kernel input buffer
	 * 
	 * @param buffer triangle buffer
	 * @return internal OpenCL call status
	 */
	cl_status setTriangleInputBuffer(const Buffer& buffer);

	/**
	 * @brief set the triangle count
	 * 
	 * This argument should be set to the amount of triangles that should be rendered
	 * @param count rendered triangle count
	 * @return internal OpenCL call status
	 */
	cl_status setTriangleCount(const nr_uint count);

	/**
	 * @brief set the target screen dimensions
	 * 
	 * @param screenDim screen dimensions (in pixels)
	 * @return internal OpenCL call status
	 */
	cl_status setScreenDimension(const ScreenDimension& screenDim);

	/**
	 * @brief set the kernel bin queue config
	 * 
	 * This argument sets the size of each bin and max element count per each bin queue
	 * @param config bin queue configuration
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueueConfig(const BinQueueConfig& config);

	/**
	 * @brief set the overflow notifier
	 * 
	 * In case of a queue overflow (too many triangles in a single bin queue), this buffer will be set.
	 * @param overflow overflow detection buffer
	 * @return internal OpenCL call status
	 */
	cl_status setOvereflowNotifier(const Buffer& overflow);

	/**
	 * @brief sets the actual bin queues buffer
	 * 
	 * @param binQueues bin raster queues buffer
	 * @return internal OpenCL call status
	 */
	cl_status setBinQueuesBuffer(const Buffer& binQueues);

	/**
	 * @brief sets the batch index buffer
	 * 
	 * The batch index is a global variable used internally by the bin rasterize kernel, and sholdn't be modified by the host.
	 * Should hold enough space for an unsigned int, or 4 bytes
	 * @param batchIndex batch index buffer
	 * @return internal OpenCL call status
	 */
	cl_status setGlobalBatchIndex(const Buffer& batchIndex);

	/**
	 * @brief calculates the execution range for this stage
	 *
	 * @note this function will raise no erros; an invalid dimension will only raise errors when enqueueing to the command queue
	 * @see setExecutionRange(const nr_uint, const nr_uint, const nr_uint)
	 *
	 * @param screenDim target screen size
	 * @param config bin queue configuration
	 * @param workGroupCount count of parallel work groups
	 */
	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount);

	/**
	 * @brief set the execution range for this stage
	 *
	 * @note this function will raise no erros; an invalid dimension will only raise errors when enqueueing to the command queue
	 *
	 * @param binCountX amount of bins on the X axis
	 * @param binCountY amount of bins on the Y axis
	 * @param workGroupCount count of parallel work groups
	 */
	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount);
};

}

}