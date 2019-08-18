#include "Pipeline.h"

#include "../utils/converters.h"
#include "../utils/rendermath.h"

#include "Source.h"

#include <iostream>

namespace nr
{

const ScreenDimension Pipeline::MAX_SCREEN_DIM = { 2048, 2048 };

Pipeline::Pipeline(const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount)
	: m_binQueueConfig(config), m_binRasterWorkGroupCount(binRasterWorkGroupCount), m_clearColor{ 0, 0, 0, 0 }, m_clearDepth(1.0f), m_renderDimension(0), m_screenDimension{ 0, 0 }
{
}

Pipeline::Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, cl_status* err)
	:  Pipeline(config, binRasterWorkGroupCount)
{	
	init(context, device, queue, dim, err);
}

Pipeline::Pipeline()
	: Pipeline({64, 64, 255}, 1)
{
}

void Pipeline::init(const Context& context, const Device& device, const CommandQueue& queue, const nr_uint dim, cl_status* err)
{
	m_context = context;
	m_device = device;
	m_commandQueue = queue;

	m_overflowNotifier = Buffer::make<nr_bool>(context, CL_MEM_READ_WRITE, 1, err);
	if (error::isFailure(*err)) return;

	m_globalBatchIndex = Buffer::make<nr_uint>(context, CL_MEM_READ_WRITE, 1, err);
	if (error::isFailure(*err)) return;

	*err = setRenderDimension(dim);
	if (error::isFailure(*err)) return;

	*err = preallocate(MAX_SCREEN_DIM, m_binQueueConfig, m_binRasterWorkGroupCount);
}

cl_status Pipeline::setRenderDimension(const nr_uint dim)
{
	m_renderDimension = dim;

	cl_status ret = CL_SUCCESS;
	auto pret = &ret;

	detail::Source s(m_context);
	ret = s.build(m_device, dim, true);
	if (error::isFailure(ret)) return ret;

	m_binRaster = s.binRasterizer();
	if (error::isFailure(ret)) return ret;

	m_fineRaster = s.fineRasterizer();
	if (error::isFailure(ret)) return ret;

	m_simplexReduce = s.simplexReducer();
	if (error::isFailure(ret)) return ret;

	m_vertexReduce = s.vertexReducer();
	if (error::isFailure(ret)) return ret;

	m_nearPlane = Buffer::make<nr_float>(m_context, CL_MEM_READ_WRITE, dim, pret);
	if (error::isFailure(ret)) return ret;
	m_farPlane = Buffer::make<nr_float>(m_context, CL_MEM_READ_WRITE, dim, pret);
	if (error::isFailure(ret)) return ret;

	ret = m_vertexReduce.setNearPlaneBuffer(m_nearPlane);
	if (error::isFailure(ret)) return ret;
	ret = m_vertexReduce.setFarPlaneBuffer(m_farPlane);
	if (error::isFailure(ret)) return ret;

	ret = m_binRaster.setBinQueueConfig(m_binQueueConfig);
	if (error::isFailure(ret)) return ret;
	ret = m_binRaster.setGlobalBatchIndex(m_globalBatchIndex);
	if (error::isFailure(ret)) return ret;
	ret = m_binRaster.setOvereflowNotifier(m_overflowNotifier);
	if (error::isFailure(ret)) return ret;
	
	ret = m_fineRaster.setBinQueuesConfig(m_binQueueConfig);
	if (error::isFailure(ret)) return ret;
	return m_fineRaster.setBinningWorkGroupCount(m_binRasterWorkGroupCount);
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
	if (screenDim.getTotalSize() > MAX_SCREEN_DIM.getTotalSize())
	{
		return CL_OUT_OF_RESOURCES;
	}

	m_screenDimension = screenDim;

	auto ret = m_binRaster.setScreenDimension(screenDim);
	if (error::isFailure(ret)) return ret;
	return m_fineRaster.setScreenDimensions(screenDim);
}

cl_status Pipeline::preallocate(const ScreenDimension& screenDim, const detail::BinQueueConfig& config, const nr_uint binRasterWorkGroupCount)
{
	cl_status err = CL_SUCCESS;
	auto perr = &err;

	const auto binCount = detail::getBinCount(screenDim, config);
	
	m_binQueues = Buffer::make<nr_uint>(m_context, CL_MEM_READ_WRITE, binCount.first * binCount.second * binRasterWorkGroupCount * (config.queueSize + 1), perr);
	if (error::isFailure(err)) return err;
	m_binRaster.setBinQueuesBuffer(m_binQueues);
	if (error::isFailure(err)) return err;
	m_fineRaster.setBinQueuesBuffer(m_binQueues);
	if (error::isFailure(err)) return err;
	
	m_frame.color = Buffer::make<RawColorRGBA>(m_context, CL_MEM_READ_WRITE, screenDim.getTotalSize(), perr);
	if (error::isFailure(err)) return err;
	m_frame.depth = Buffer::make<Depth>(m_context, CL_MEM_READ_WRITE, screenDim.getTotalSize(), perr);
	if (error::isFailure(err)) return err;
	return m_fineRaster.setFrameBuffer(m_frame);
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

cl_status Pipeline::render(const VertexBuffer& primitives, const Primitive& primitiveType, const nr_uint primitiveCount)
{
	if (primitiveType != Primitive::SIMPLEX) return CL_INVALID_VALUE;

	cl_status err = CL_SUCCESS;
	auto perr = &err;
	nr_bool overflow;

	auto elementCount = primitives.vertecies.count<nr_float>(perr);
	if (error::isFailure(err)) return err;

	if (elementCount < nr_size(primitiveCount) * (nr_size(m_renderDimension) + 1) * nr_size(m_renderDimension))
	{
		return CL_INVALID_VALUE;
	}

	m_binRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, m_binRasterWorkGroupCount);
	m_fineRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, m_binRasterWorkGroupCount);
	m_simplexReduce.setExecutionRange(primitiveCount);
	m_vertexReduce.setExecutionRange(primitiveCount * m_renderDimension);

	m_vertexReduce.setSimplexInputBuffer(primitives);
	if (error::isFailure(err)) return err;
	m_vertexReduce.setSimplexOutputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_simplexReduce.setSimplexInputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_simplexReduce.setTriangleOutputBuffer(primitives.reducedSimplices);
	if (error::isFailure(err)) return err;
	m_binRaster.setTriangleInputBuffer(primitives.reducedSimplices);
	if (error::isFailure(err)) return err;
	m_binRaster.setTriangleCount(detail::triangleCount(m_renderDimension, primitiveCount));
	if (error::isFailure(err)) return err;
	m_fineRaster.setTriangleInputBuffer(primitives.reducedSimplices);
	if (error::isFailure(err)) return err;

	err = m_commandQueue.enqueueDispatchCommand(m_vertexReduce);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue vertex reduce! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}
	err = m_commandQueue.enqueueDispatchCommand(m_simplexReduce);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue simplex reduce! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}
	err = m_commandQueue.enqueueDispatchCommand(m_binRaster);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue bin raster! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}
	err = m_commandQueue.enqueueDispatchCommand(m_fineRaster);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue fine raster! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}
	err = m_commandQueue.enqueueBufferReadCommand(m_overflowNotifier, false, 1, &overflow);
	if (error::isFailure(err)) return err;
	err = m_commandQueue.await();
	if (error::isFailure(err))
	{
		std::cerr << "Could not process command queue! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}

	if (overflow)
	{
		//return CL_OUT_OF_RESOURCES;
		std::cout << "Overflow detected!\n";
	}

	return err;
}

cl_status Pipeline::copyFrameBuffer(RawColorRGBA* bitmap) const
{
	return m_commandQueue.enqueueBufferReadCommand(m_frame.color, true, m_screenDimension.getTotalSize(), bitmap);
}

}
