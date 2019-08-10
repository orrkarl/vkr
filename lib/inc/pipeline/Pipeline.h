#pragma once

#include "../base/Context.h"
#include "../base/CommandQueue.h"

#include "../kernels/Source.h"
#include "../kernels/bin_rasterizer.cl.h"
#include "../kernels/fine_rasterizer.cl.h"
#include "../kernels/simplex_reducing.cl.h"
#include "../kernels/vertex_shading.cl.h"

#include "../rendering/Render.h"

namespace nr
{

class Pipeline
{
public:
	Pipeline(const Context& context, const CommandQueue& queue, cl_status* err);

	cl_status setRenderDimension(const nr_uint dim);

	void setClearColor(const RawColorRGBA& color);

	void setClearDepth(const Depth& depth);

	cl_status viewport(const ScreenDimension& screenDim);

	cl_status clear() const;

	cl_status setNearPlane(const nr_float* near) const;

	cl_status setFarPlane(const nr_float* far) const;

	cl_status render(const Buffer& primitives, const NRPrimitive& primitiveType, const nr_uint primitiveCount);

	cl_status copyFrameBuffer(RawColorRGBA* bitmap) const;
	
private:
	cl_status clearDepthBuffer() const;
								 
	cl_status clearColorBuffer() const;

	detail::BinQueueConfig			m_binQueueConfig;
	Buffer							m_binQueues;
	detail::BinRasterizerKernel		m_binRaster;
	RawColorRGBA					m_clearColor;
	Depth							m_clearDepth;
	CommandQueue					m_commandQueue;
	Context							m_context;
	nr_uint							m_renderDimension;
	Buffer							m_farPlane;
	detail::FineRasterizerKernel	m_fineRaster;
	FrameBuffer						m_frame;
	nr_uint							m_lastPrimitiveCount;
	Buffer							m_nearPlane;
	Buffer							m_overflowNotifier;
	Buffer							m_reducedSimplexes;
	Buffer							m_reducedVertecies;
	ScreenDimension					m_screenDimension;
	detail::SimplexReduceKernel		m_simplexReduce;
	detail::VertexReduceKernel		m_vertexReduce;
};

}
