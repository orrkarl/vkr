/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief main rasterization interface
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "../base/Context.h"
#include "../base/CommandQueue.h"

#include "BinRasterizer.h"
#include "FineRasterizer.h"
#include "VertexReducer.h"

#include "../rendering/Render.h"

#include "VertexBuffer.h"

namespace nr
{

/**
 * @brief Full rasterization interface. Use this class to actually rasterize simplices.
 * 
 */
class NRAPI Pipeline
{
public:
	/**
	 * @brief Constructs a ready to use pipeline
	 * 
	 * Constructs and initializes a pipeline with all the necessary configuration and data
	 * @param context parent context
	 * @param device parent device
	 * @param queue parent command queue
	 * @param dim rasterization dimension
	 * @param config bin queue config (refer to BinRasterizer for detailed explenation)
	 * @param binRasterWorkGroupCount bin rasterizer expected work group count
	 * @param batchSize bin rasterizer batch size
	 * @param[out] err internal OpenCL call status
	 */
	Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, const nr_uint batchSize, cl_status& err);

	/**
	 * @brief Construct and initializes a pipeline
	 * 
	 * @see Pipeline::Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, const nr_uint batchSize, cl_status& err);
	 * @param context parent context
	 * @param device parent device
	 * @param queue parent command queue
	 * @param dim rasterization dimension
	 * @param config bin queue config (refer to BinRasterizer for detailed explenation)
	 * @param binRasterWorkGroupCount bin rasterizer expected work group count
	 * @param[out] err internal OpenCL call status
	 */
	Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, cl_status& err);

	Pipeline(const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, const nr_uint batchSize = DEFAULT_BATCH_SIZE);

	Pipeline();

	/**
	 * @brief Set the colorbuffer default color
	 * 
	 * When Pipeline::clear() is called, the color buffer will be painted to this color
	 * @param color colorbuffer default color
	 */
	void setClearColor(const RawColorRGBA& color);

	/**
	 * @brief Set the depthbuffer default depth
	 * 
	 * When Pipeline::clear() is called, the depthbuffer will be painted to this depth
	 * @param depth depthbuffer default depth
	 */
	void setClearDepth(const Depth& depth);

	void init(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, cl_status& err);

	/**
	 * @brief Set the viewport size
	 * 
	 * @param screenDim new viewport size
	 * @return cl_status internal OpenCL call status
	 */
	cl_status viewport(const ScreenDimension& screenDim);

	/**
	 * @brief clear the framebuffer 
	 * 
	 * This function will enqueue fill commands to the framebuffer with given defaults
	 * @return cl_status internal OpenCL call status
	 */
	cl_status clear() const;

	/**
	 * @brief Sets the perspective near plane
	 * 
	 * Refer to VertexReducer for detailed explenation
	 * @param near perspective near buffer
	 * @return cl_status internal OpenCL call status
	 */
	cl_status setNearPlane(const nr_float* near) const;

	/**
	 * @brief Sets the perspective far plane
	 * 
	 * Refer to VertexReducer for detailed explenation
	 * @param far perspective far buffer
	 * @return cl_status internal OpenCL call status
	 */
	cl_status setFarPlane(const nr_float* far) const;

	/**
	 * @brief Enqueues a render command
	 * 
	 * @note This function blocks execution until rasterization is completed
	 * @param primitives render data source
	 * @param primitiveType render data type
	 * @param primitiveCount render data type
	 * @return cl_status internal OpenCL call status
	 */
	cl_status render(const VertexBuffer& primitives, const Primitive& primitiveType, const nr_uint primitiveCount);

	/**
	 * @brief Copies the framebuffer 
	 * 
	 * @note This function blocks execution until the entire buffer is copied
	 * @param bitmap host buffer containing enough place for the framebuffer
	 * @return cl_status internal OpenCL call status
	 */
	cl_status copyFrameBuffer(RawColorRGBA* bitmap) const;
	
private:
	cl_status clearDepthBuffer() const;
								 
	cl_status clearColorBuffer() const;

	cl_status preallocate(const ScreenDimension& screenDim, const detail::BinQueueConfig& config, const nr_uint binRasterWorkGroupCounts);

	cl_status setRenderDimension(const nr_uint dim);

	static const nr_uint		 DEFAULT_BATCH_SIZE;
	static const ScreenDimension MAX_SCREEN_DIM;

	const nr_uint					m_batchSize;
	const detail::BinQueueConfig	m_binQueueConfig;
	Buffer							m_binQueues;
	detail::BinRasterizer			m_binRaster;
	const nr_uint					m_binRasterWorkGroupCount;
	RawColorRGBA					m_clearColor;
	Depth							m_clearDepth;
	CommandQueue					m_commandQueue;
	Context							m_context;
	Device							m_device;
	Buffer							m_farPlane;
	Buffer							m_globalBatchIndex;
	nr_uint							m_renderDimension;
	detail::FineRasterizer			m_fineRaster;
	FrameBuffer						m_frame;
	Buffer							m_nearPlane;
	Buffer							m_overflowNotifier;
	ScreenDimension					m_screenDimension;
	detail::VertexReducer			m_vertexReduce;
};

}
