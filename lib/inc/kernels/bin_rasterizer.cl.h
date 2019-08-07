#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Dispatch.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

#include <cmath>

namespace nr
{

namespace detail
{

NRAPI extern const char* BIN_RASTER_KERNEL_NAME;

class BinRasterizerKernel : private TypesafeKernel<Buffer, nr_uint, ScreenDimension, BinQueueConfig, Buffer, Buffer, Buffer>, public Dispatch<BinRasterizerKernel>
{
public:
	BinRasterizerKernel::BinRasterizerKernel(const Module& module, cl_status* err = nullptr)
		: TypesafeKernel(module, BIN_RASTER_KERNEL_NAME, err)
	{
	}

	BinRasterizerKernel::BinRasterizerKernel()
		: TypesafeKernel()
	{
	}

	cl_status setTriangleInputBuffer(const Buffer& buffer)
	{
		return setArg<TRIANGLE_BUFFER>(buffer);
	}

	cl_status setTriangleCount(const nr_uint count)
	{
		return setArg<TRIANGLE_COUNT>(count);
	}

	cl_status setScreenDimension(const ScreenDimension& screenDim)
	{
		return setArg<SCREEN_DIMENSION>(screenDim);
	}

	cl_status setBinQueueConfig(const BinQueueConfig& config)
	{
		return setArg<BIN_QUEUE_CONFIG>(config);
	}

	cl_status setOvereflowNotifier(const Buffer& overflow)
	{
		return setArg<OVERFLOW_BUFFER>(overflow);
	}

	cl_status setBinQueuesBuffer(const Buffer& binQueues)
	{
		return setArg<BIN_QUEUES_BUFFER>(binQueues);
	}

	cl_status setGlobalBatchIndex(const Buffer& batchIndex)
	{
		return setArg<BATCH_INDEX>(batchIndex);
	}

	cl_status consume(const CommandQueue& q) const
	{
		return q.enqueueKernelCommand(*this, m_range);
	}

	void setExecutionRange(const ScreenDimension& screenDim, const BinQueueConfig& config, const nr_uint workGroupCount)
	{
		nr_uint binCountX = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.width) / config.binWidth));
		nr_uint binCountY = static_cast<nr_uint>(std::ceil(static_cast<nr_float>(screenDim.height) / config.binHeight));
		return setExecutionRange(binCountX, binCountY, workGroupCount);
	}

	void setExecutionRange(const nr_uint binCountX, const nr_uint binCountY, const nr_uint workGroupCount)
	{
		m_range.global.x = workGroupCount * binCountX;
		m_range.global.y = binCountY;

		m_range.local.x = binCountX;
		m_range.local.y = binCountY;
	}

	NDExecutionRange<2> getExecutionRange() const
	{
		return m_range;
	}

	using TypesafeKernel::operator cl_kernel;

private:
	NDExecutionRange<2> m_range;

	static constexpr const nr_uint TRIANGLE_BUFFER = 0;
	static constexpr const nr_uint TRIANGLE_COUNT = 1;
	static constexpr const nr_uint SCREEN_DIMENSION = 2;
	static constexpr const nr_uint BIN_QUEUE_CONFIG = 3;
	static constexpr const nr_uint OVERFLOW_BUFFER = 4;
	static constexpr const nr_uint BIN_QUEUES_BUFFER = 5;
	static constexpr const nr_uint BATCH_INDEX = 6;
};

namespace clcode
{

NRAPI extern const char* bin_rasterizer;

}

}

}