#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Dispatch.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* SIMPLEX_REDUCE_KERNEL_NAME;

class SimplexReduceKernel : private TypesafeKernel<Buffer, Buffer>, public Dispatch<SimplexReduceKernel>
{
public:
	SimplexReduceKernel(const Module& module, cl_status* err = nullptr)
		: TypesafeKernel<Buffer, Buffer>(module, SIMPLEX_REDUCE_KERNEL_NAME, err)
	{
	}

	SimplexReduceKernel()
		: TypesafeKernel()
	{
	}

	cl_status setSimplexInputBuffer(const Buffer& in)
	{
		return setArg<INPUT_BUFFER>(in);
	}

	cl_status setTriangleOutputBuffer(const Buffer& out)
	{
		return setArg<OUTPUT_BUFFER>(out);
	}

	cl_status consume(const CommandQueue& q) const
	{
		return q.enqueueKernelCommand<1>(*this, m_range);
	}

	void setExecutionRange(const nr_uint primitiveCount)
	{
		m_range.global.x = primitiveCount;
		m_range.local.x = 1;
	}

	NDExecutionRange<1> getExecutionRange() const
	{
		return m_range;
	}
	
	using TypesafeKernel<Buffer, Buffer>::operator cl_kernel;

private:
	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint OUTPUT_BUFFER = 1;

	NDExecutionRange<1> m_range;
};

namespace clcode
{

// Reducing simplexes to their triangles
NRAPI extern const char* simplex_reducing;

}

}

}
