#pragma once

#include "../predefs.h"

#include "../kernels/vertex_reducer.cl.h"
#include "../utils/StandardDispatch.h"

namespace nr
{

namespace detail
{

class VertexReducer : public StandardDispatch<1, Buffer, Buffer, Buffer, Buffer>
{
public:
	VertexReducer(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, VERTEX_REDUCE_KERNEL_NAME, err)
	{
	}

	VertexReducer()
		: StandardDispatch()
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

	void setExecutionRange(const nr_uint vertexCount)
	{
		range.global.x = vertexCount;
		range.local.x = 1;
	}

	using TypesafeKernel::operator cl_kernel;

private:
	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint NEAR_PLANE_BUFFER = 1;
	static constexpr const nr_uint FAR_PLANE_BUFFER = 2;
	static constexpr const nr_uint OUTPUT_BUFFER = 3;
};

}

}