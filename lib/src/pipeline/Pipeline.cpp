#include <pipeline/Pipeline.h>

#include <utils/rendermath.h>

namespace nr
{

Pipeline::Pipeline(const Context& context, const CommandQueue& queue, cl_status* err)
	: m_binQueueConfig{ 64, 64, 256 }, m_clearColor{ 0, 0, 0, 0 }, m_clearDepth(1.0f), m_commandQueue(queue), m_context(context), m_renderDimension(0), m_lastPrimitiveCount(0), m_screenDimension{ 0, 0 }
{
}

cl_status Pipeline::setRenderDimension(const nr_uint dim)
{
	if (dim < m_renderDimension)
	{
		m_renderDimension = dim;
		return CL_SUCCESS;
	}

	cl_status ret = CL_SUCCESS;
	auto pret = &ret;

	m_nearPlane = Buffer::make<nr_float>(m_context, CL_MEM_READ_WRITE, 1, pret);
	if (error::isFailure(ret)) return ret;
	m_farPlane = Buffer::make<nr_float>(m_context, CL_MEM_READ_WRITE, 1, pret);
	if (error::isFailure(ret)) return ret;
}

void Pipeline::setClearColor(const RawColorRGBA& color)
{
	m_clearColor = color;
}

void Pipeline::setClearDepth(const Depth& depth)
{
	m_clearDepth = 1 / depth;
}

cl_status Pipeline::viewport(const ScreenDimension& screenDim)
{
	return cl_status();
}

cl_status Pipeline::clear() const
{
	cl_status ret = CL_SUCCESS;
	ret = clearColorBuffer();

	if (error::isFailure(ret)) return ret;

	return clearDepthBuffer();
}

cl_status Pipeline::clearDepthBuffer() const
{
	return m_commandQueue.enqueueBufferFillCommand<Depth>(m_frame.depth, m_clearDepth, m_screenDimension.getTotalSize());
}

cl_status Pipeline::clearColorBuffer() const
{
	return m_commandQueue.enqueueBufferFillCommand<RawColorRGBA>(m_frame.color, m_clearColor, m_screenDimension.getTotalSize());
}

cl_status Pipeline::setNearPlane(const nr_float* near) const
{
	return m_commandQueue.enqueueBufferWriteCommand(m_nearPlane, false, m_renderDimension, near);
}

cl_status Pipeline::setFarPlane(const nr_float* far) const
{
	return m_commandQueue.enqueueBufferWriteCommand(m_farPlane, false, m_renderDimension, far);
}

cl_status Pipeline::render(const Buffer& primitives, const NRPrimitive& primitiveType, const nr_uint primitiveCount)
{
	if (primitiveType != NRPrimitive::SIMPLEX) return CL_INVALID_VALUE;

	cl_status err = CL_SUCCESS;
	auto perr = &err;
	nr_bool overflow;

	if (primitiveCount > m_lastPrimitiveCount)
	{
		m_reducedSimplexes = Buffer(m_context, CL_MEM_READ_WRITE, nr_size(detail::triangleCount(m_renderDimension, primitiveCount)) * 3 * (nr_size(m_renderDimension) + 1) * sizeof(nr_float), perr);
		if (error::isFailure(err)) return err;

		m_reducedVertecies = Buffer(m_context, CL_MEM_READ_WRITE, (nr_size(m_renderDimension) + 1) * nr_size(m_renderDimension) * nr_size(primitiveCount) * sizeof(nr_float), perr);
		if (error::isFailure(err)) return err;

		m_lastPrimitiveCount = primitiveCount;
	}

	constexpr auto binRasterWorkGroups = 1u;
	m_binRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, binRasterWorkGroups);
	m_fineRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, binRasterWorkGroups);
	m_simplexReduce.setExecutionRange(primitiveCount);
	m_vertexReduce.setExecutionRange(primitiveCount * m_renderDimension);

	m_vertexReduce.setSimplexInputBuffer(primitives);
	m_vertexReduce.setSimplexOutputBuffer(m_reducedVertecies);
	m_simplexReduce.setSimplexInputBuffer(m_reducedVertecies);
	m_simplexReduce.setTriangleOutputBuffer(m_reducedSimplexes);
	m_binRaster.setTriangleInputBuffer(m_reducedSimplexes);
	m_fineRaster.setTriangleInputBuffer(m_reducedSimplexes);

	err = m_commandQueue.enqueueDispatchCommand(m_vertexReduce);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.enqueueDispatchCommand(m_simplexReduce);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.enqueueDispatchCommand(m_binRaster);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.enqueueDispatchCommand(m_fineRaster);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.enqueueBufferReadCommand(m_overflowNotifier, false, 1, &overflow);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.await();
	if (error::isFailure(err)) return err;

	if (overflow) return CL_OUT_OF_RESOURCES;

	return err;
}

cl_status Pipeline::copyFrameBuffer(RawColorRGBA* bitmap) const
{
	return m_commandQueue.enqueueBufferReadCommand(m_frame.color, true, bitmap);
}

}
