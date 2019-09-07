#include "VertexReducer.h"

namespace nr
{

namespace detail
{

constexpr nr_uint INPUT_BUFFER	= 0;
constexpr nr_uint NEAR_PLANE	= 1;
constexpr nr_uint FAR_PLANE		= 2;
constexpr nr_uint ASPECT_RATIO	= 3;
constexpr nr_uint OUTPUT_BUFFER = 4;

VertexReducer::VertexReducer(const Module& module, cl_status& err)
	: StandardDispatch(module, VERTEX_REDUCE_KERNEL_NAME, err)
{
}

VertexReducer::VertexReducer()
	: StandardDispatch()
{
}

cl_status VertexReducer::setSimplexInputBuffer(const Buffer& in)
{
	return setArg<INPUT_BUFFER>(in);
}

cl_status VertexReducer::setNearPlane(const cl_float3& near)
{
	return setArg<NEAR_PLANE>(near);
}

cl_status VertexReducer::setFarPlane(const cl_float3& far)
{
	return setArg<FAR_PLANE>(far);
}

cl_status VertexReducer::setAspectRatio(const ScreenDimension& screenDim)
{
	if (screenDim.width == 0 || screenDim.height == 0)
	{
		return CL_INVALID_VALUE;
	}

	return setArg<ASPECT_RATIO>(static_cast<nr_float>(screenDim.height) / screenDim.width);
}

cl_status VertexReducer::setSimplexOutputBuffer(const Buffer& out)
{
	return setArg<OUTPUT_BUFFER>(out);
}

void VertexReducer::setExecutionRange(const nr_uint vertexCount)
{
	range.global.x = vertexCount;
	range.local.x = 1;
}

}

}