#pragma once

#include "../base/Context.h"
#include "../base/CommandQueue.h"

#include "../kernels/bin_rasterizer.cl.h"
#include "../kernels/fine_rasterizer.cl.h"
#include "../kernels/simplex_reducing.cl.h"
#include "../kernels/vertex_shading.cl.h"

#include "../rendering/Render.h"

#include "VertexBuffer.h"

namespace nr
{

class Pipeline
{
public:
	Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, cl_status* err);

	cl_status setRenderDimension(const nr_uint dim);

	void setClearColor(const RawColorRGBA& color);

	void setClearDepth(const Depth& depth);

	cl_status viewport(const ScreenDimension& screenDim);

	cl_status clear() const;

	cl_status setNearPlane(const nr_float* near) const;

	cl_status setFarPlane(const nr_float* far) const;

	cl_status render(const VertexBuffer& primitives, const NRPrimitive& primitiveType, const nr_uint primitiveCount);

	cl_status copyFrameBuffer(RawColorRGBA* bitmap) const;
	
private:
	cl_status clearDepthBuffer() const;
								 
	cl_status clearColorBuffer() const;

	cl_status preallocate(const ScreenDimension& screenDim, const detail::BinQueueConfig& config, const nr_uint binRasterWorkGroupCounts);

	static const ScreenDimension MAX_SCREEN_DIM;

	const detail::BinQueueConfig	m_binQueueConfig;
	Buffer							m_binQueues;
	detail::BinRasterizerKernel		m_binRaster;
	const nr_uint					m_binRasterWorkGroupCount;
	RawColorRGBA					m_clearColor;
	Depth							m_clearDepth;
	CommandQueue					m_commandQueue;
	Context							m_context;
	Device							m_device;
	Buffer							m_farPlane;
	Buffer							m_globalBatchIndex;
	nr_uint							m_renderDimension;
	detail::FineRasterizerKernel	m_fineRaster;
	FrameBuffer						m_frame;
	Buffer							m_nearPlane;
	Buffer							m_overflowNotifier;
	ScreenDimension					m_screenDimension;
	detail::SimplexReduceKernel		m_simplexReduce;
	detail::VertexReduceKernel		m_vertexReduce;
};

}
