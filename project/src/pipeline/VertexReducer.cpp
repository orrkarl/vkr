#include "VertexReducer.h"

namespace nr
{

namespace detail
{

constexpr nr_uint INPUT_BUFFER	= 0;
constexpr nr_uint ASPECT_RATIO	= 1;
constexpr nr_uint SCALED_FOV	= 2;
constexpr nr_uint Z_NEAR		= 3;
constexpr nr_uint Z_FAR			= 4;
constexpr nr_uint OUTPUT_BUFFER = 5;

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

cl_status VertexReducer::setAspectRatio(const ScreenDimension& screenDimension)
{
	return setArg<ASPECT_RATIO>(static_cast<nr_float>(screenDimension.height) / screenDimension.width);
}

cl_status VertexReducer::setFieldOfView(const nr_float angle)
{
	return setArg<SCALED_FOV>(1 / std::tan(angle / 2));
}

cl_status VertexReducer::setZNearPlane(const nr_float zNear)
{
	return setArg<Z_NEAR>(zNear);
}

cl_status VertexReducer::setZFarPlane(const nr_float zFar)
{
	return setArg<Z_FAR>(zFar);
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