#pragma once

#include "../predefs.h"

#include "../base/Context.h"
#include "../base/CommandQueue.h"

#include "BinRasterizer.h"
#include "FineRasterizer.h"
#include "SimplexReducer.h"
#include "VertexReducer.h"

#include "../rendering/Render.h"

#include "VertexBuffer.h"

namespace nr
{

class NRAPI Pipeline
{
public:
	Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, cl_status* err);

	Pipeline(const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount);

	Pipeline();

	void setClearColor(const RawColorRGBA& color);

	void setClearDepth(const Depth& depth);

	void init(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, cl_status* err);

	cl_status viewport(const ScreenDimension& screenDim);

	cl_status clear() const;

	cl_status setNearPlane(const nr_float* near) const;

	cl_status setFarPlane(const nr_float* far) const;

	cl_status render(const VertexBuffer& primitives, const Primitive& primitiveType, const nr_uint primitiveCount);

	cl_status copyFrameBuffer(RawColorRGBA* bitmap) const;
	
private:
	cl_status clearDepthBuffer() const;
								 
	cl_status clearColorBuffer() const;

	cl_status preallocate(const ScreenDimension& screenDim, const detail::BinQueueConfig& config, const nr_uint binRasterWorkGroupCounts);

	cl_status setRenderDimension(const nr_uint dim);

	static const ScreenDimension MAX_SCREEN_DIM;

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
	detail::SimplexReducer			m_simplexReduce;
	detail::VertexReducer			m_vertexReduce;
};

}
