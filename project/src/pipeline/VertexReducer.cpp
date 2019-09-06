#include "VertexReducer.h"

namespace nr
{

namespace detail
{

constexpr nr_uint INPUT_BUFFER = 0;
constexpr nr_uint NEAR_PLANE_BUFFER = 1;
constexpr nr_uint FAR_PLANE_BUFFER = 2;
constexpr nr_uint OUTPUT_BUFFER = 3;

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

cl_status VertexReducer::setNearPlaneBuffer(const Buffer& near)
{
	return setArg<NEAR_PLANE_BUFFER>(near);
}

cl_status VertexReducer::setFarPlaneBuffer(const Buffer& far)
{
	return setArg<FAR_PLANE_BUFFER>(far);
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