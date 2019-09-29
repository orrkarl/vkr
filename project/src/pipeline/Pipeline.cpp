#include "Pipeline.h"

#include "../utils/converters.h"
#include "../utils/rendermath.h"

#include "Source.h"

#include <iostream>

namespace nr
{

const nr_uint Pipeline::DEFAULT_BATCH_SIZE = 256;
const ScreenDimension Pipeline::MAX_SCREEN_DIM = { 2048, 2048 };
	

Pipeline::Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, const nr_uint batchSize, cl_status& err)
	:
	m_binQueueConfig(config), m_binRasterWorkGroupCount(binRasterWorkGroupCount), m_batchSize(batchSize),
	m_clearColor{ 0, 0, 0, 0 }, m_clearDepth(1.0f),
	m_commandQueue(queue),
	m_screenDimension{ 0, 0 }
{

	m_overflowNotifier = Buffer::make<nr_bool>(context, CL_MEM_READ_WRITE, 1, err);
	if (error::isFailure(err)) return;

	m_globalBatchIndex = Buffer::make<nr_uint>(context, CL_MEM_READ_WRITE, 1, err);
	if (error::isFailure(err)) return;

	auto src = detail::Source(context, err);
	if (error::isFailure(err)) return;

	err = src.build(device, batchSize, true);
	if (error::isFailure(err)) return;

	m_binRaster = src.binRasterizer(err);
	if (error::isFailure(err)) return;

	m_fineRaster = src.fineRasterizer(err);
	if (error::isFailure(err)) return;

	m_vertexReduce = src.vertexReducer(err);
	if (error::isFailure(err)) return;

	m_vertexPostProcessor = src.vertexPostProcessor(err);
	if (error::isFailure(err)) return;
	
	err = m_binRaster.setBinQueueConfig(m_binQueueConfig);
	if (error::isFailure(err)) return;
	err = m_binRaster.setGlobalBatchIndex(m_globalBatchIndex);
	if (error::isFailure(err)) return;
	err = m_binRaster.setOvereflowNotifier(m_overflowNotifier);
	if (error::isFailure(err)) return;

	err = m_fineRaster.setBinQueuesConfig(m_binQueueConfig);
	if (error::isFailure(err)) return;
	err = m_fineRaster.setBinningWorkGroupCount(m_binRasterWorkGroupCount);
	if (error::isFailure(err)) return;

	err = preallocate(context, MAX_SCREEN_DIM, m_binQueueConfig, m_binRasterWorkGroupCount);

}

Pipeline::Pipeline(const Context& context, const Device& device, const CommandQueue& queue, const detail::BinQueueConfig config, const nr_uint binRasterWorkGroupCount, cl_status& err)
	:  Pipeline(context, device, queue, config, binRasterWorkGroupCount, DEFAULT_BATCH_SIZE, err)
{	
}

Pipeline::Pipeline()
	:
	m_binQueueConfig{0, 0, 0}, m_binRasterWorkGroupCount(0), m_batchSize(0),
	m_clearColor{ 0, 0, 0, 0 }, m_clearDepth(0.0f),
	m_screenDimension{ 0, 0 }
{
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
	ret = m_vertexReduce.setAspectRatio(screenDim);
	if (error::isFailure(ret)) return ret;
	return m_fineRaster.setScreenDimensions(screenDim);
}

cl_status Pipeline::preallocate(const Context& context, const ScreenDimension& screenDim, const detail::BinQueueConfig& config, const nr_uint binRasterWorkGroupCount)
{
	cl_status err = CL_SUCCESS;

	const auto binCount = detail::getBinCount(screenDim, config);
	
	m_binQueues = Buffer::make<nr_uint>(context, CL_MEM_READ_WRITE, binCount.first * binCount.second * binRasterWorkGroupCount * (config.queueSize + 1), err);
	if (error::isFailure(err)) return err;
	m_binRaster.setBinQueuesBuffer(m_binQueues);
	if (error::isFailure(err)) return err;
	m_fineRaster.setBinQueuesBuffer(m_binQueues);
	if (error::isFailure(err)) return err;
	
	m_frame.color = Buffer::make<RawColorRGBA>(context, CL_MEM_READ_WRITE, screenDim.getTotalSize(), err);
	if (error::isFailure(err)) return err;
	m_frame.depth = Buffer::make<Depth>(context, CL_MEM_READ_WRITE, screenDim.getTotalSize(), err);
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

cl_status Pipeline::setFieldOfView(const nr_float angle)
{
	return m_vertexReduce.setFieldOfView(angle);
}

cl_status Pipeline::setZNearPlane(const nr_float zNear)
{
	return m_vertexReduce.setZNearPlane(zNear);
}

cl_status Pipeline::setZFarPlane(const nr_float zFar)
{
	return m_vertexReduce.setZFarPlane(zFar);
}

cl_status Pipeline::clearDepthBuffer() const
{
	return m_commandQueue.enqueueBufferFillCommand<Depth>(m_frame.depth, m_clearDepth, m_screenDimension.getTotalSize());
}

cl_status Pipeline::clearColorBuffer() const
{
	return m_commandQueue.enqueueBufferFillCommand<RawColorRGBA>(m_frame.color, m_clearColor, m_screenDimension.getTotalSize());
}


cl_status Pipeline::render(const VertexBuffer& primitives, const Primitive& primitiveType, const nr_uint primitiveCount)
{
	if (primitiveType != Primitive::TRIANGLE) return CL_INVALID_VALUE;

	cl_status err = CL_SUCCESS;
	nr_bool overflow;

	auto elementCount = primitives.vertecies.count<nr_float>(err);
	if (error::isFailure(err)) return err;

	if (elementCount < primitiveCount * sizeof(Triangle) / sizeof(nr_float))
	{
		return CL_INVALID_VALUE;
	}

	m_vertexReduce.setExecutionRange(primitiveCount * 3);
	m_vertexPostProcessor.setExecutionRange(primitiveCount);
	m_binRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, m_binRasterWorkGroupCount);
	m_fineRaster.setExecutionRange(m_screenDimension, m_binQueueConfig, m_binRasterWorkGroupCount);

	m_vertexReduce.setVertexInputBuffer(primitives);
	if (error::isFailure(err)) return err;
	m_vertexReduce.setVertexOutputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_vertexPostProcessor.setTriangleInputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_vertexPostProcessor.setTriangleRecordOutputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_binRaster.setTriangleInputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;
	m_binRaster.setTriangleCount(primitiveCount);
	if (error::isFailure(err)) return err;
	m_fineRaster.setTriangleInputBuffer(primitives.reducedVertecies);
	if (error::isFailure(err)) return err;

	err = m_commandQueue.enqueueDispatchCommand(m_vertexReduce);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue vertex reduce! " << utils::stringFromCLError(err) << std::endl;
		return err;
	}
	err = m_commandQueue.enqueueDispatchCommand(m_vertexPostProcessor);
	if (error::isFailure(err))
	{
		std::cerr << "Could not enqueue vertex post processor! " << utils::stringFromCLError(err) << std::endl;
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
		return CL_OUT_OF_RESOURCES;
	}

	return err;
}

cl_status Pipeline::copyFrameBuffer(RawColorRGBA* bitmap) const
{
	return m_commandQueue.enqueueBufferReadCommand(m_frame.color, true, m_screenDimension.getTotalSize(), bitmap);
}

}
