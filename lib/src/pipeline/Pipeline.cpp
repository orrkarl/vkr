#include <pipeline/Pipeline.h>

nr::Pipeline::Pipeline(const Context& context, const CommandQueue& queue, cl_status* err)
{
}

cl_status nr::Pipeline::setRenderDimension(const nr_uint dim)
{
	return cl_status();
}

void nr::Pipeline::setClearColor(const RawColorRGBA& color)
{
	m_clearColor = color;
}

void nr::Pipeline::setClearDepth(const Depth& depth)
{
	m_clearDepth = depth;
}

cl_status nr::Pipeline::viewport(const ScreenDimension& screenDim)
{
	return cl_status();
}

cl_status nr::Pipeline::clear() const
{
	return cl_status();
}

cl_status nr::Pipeline::setNearPlane(const nr_float* near) const
{
	return m_commandQueue.enqueueBufferWriteCommand(m_near, false, m_renderDimension, near);
}

cl_status nr::Pipeline::setFarPlane(const nr_float* far) const
{
	return m_commandQueue.enqueueBufferWriteCommand(m_far, false, m_renderDimension, far);
}

cl_status nr::Pipeline::render(const Buffer& primitives, const NRPrimitive& primitiveType, const nr_uint primitiveCount) const
{
	return cl_status();
}

cl_status nr::Pipeline::copyFrameBuffer(RawColorRGBA* bitmap) const
{
	return m_commandQueue.enqueueBufferReadCommand(m_frame.color, false, bitmap);
}

cl_status nr::Pipeline::fence() const
{
	return m_commandQueue.await();
}
