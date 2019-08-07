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

NRAPI extern const char* VERTEX_REDUCE_KERNEL_NAME;

class VertexReduceKernel : private TypesafeKernel<Buffer, Buffer, Buffer, Buffer>, public Dispatch<VertexReduceKernel> 
{
public:
	VertexReduceKernel(const Module& module, cl_status* err = nullptr)
		: TypesafeKernel<Buffer, Buffer, Buffer, Buffer>(module, VERTEX_REDUCE_KERNEL_NAME, err)
	{
	}

	VertexReduceKernel()
		: TypesafeKernel()
	{
	}

	cl_status setSimplexInputBuffer(const Buffer& in)
	{
		return setArg<INPUT_BUFFER>(in);
	}

	cl_status setNearPlaneBuffer(const Buffer& near)
	{
		return setArg<NEAR_PLANE_BUFFER>(near);
	}

	cl_status setFarPlaneBuffer(const Buffer& far)
	{
		return setArg<FAR_PLANE_BUFFER>(far);
	}

	cl_status setSimplexOutputBuffer(const Buffer& out)
	{
		return setArg<OUTPUT_BUFFER>(out);
	}

	cl_status consume(const CommandQueue& q) const
	{
		return q.enqueueKernelCommand(*this, m_range);
	}

	void setExecutionRange(const nr_uint vertexCount)
	{
		m_range.global.x = vertexCount;
		m_range.local.x = 1;
	}

	NDExecutionRange<1> getExecutionRange() const
	{
		return m_range;
	}

	using TypesafeKernel::operator cl_kernel;

private:
	NDExecutionRange<1> m_range;

	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint NEAR_PLANE_BUFFER = 1;
	static constexpr const nr_uint FAR_PLANE_BUFFER = 2;
	static constexpr const nr_uint OUTPUT_BUFFER = 3;
};

namespace clcode
{

NRAPI extern const char* vertex_shading;

}

}

}