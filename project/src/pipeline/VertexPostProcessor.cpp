#include "VertexPostProcessor.h"

namespace nr
{

namespace detail
{

VertexPostProcessor::VertexPostProcessor(const Module& module, cl_status& err)
	: StandardDispatch(module, TRIANGLE_SETUP_KERNEL_NAME, err)
{
}

VertexPostProcessor::VertexPostProcessor()
{
}

cl_status VertexPostProcessor::setTriangleInputBuffer(const Buffer& triangleBuffer)
{
	return setArg<0>(triangleBuffer);
}

cl_status VertexPostProcessor::setTriangleRecordOutputBuffer(const Buffer& recordBuffer)
{
	return setArg<1>(recordBuffer);
}

void VertexPostProcessor::setExecutionRange(const nr_uint triangleCount)
{
	range.global.x = triangleCount;
	range.local.x = 1;
}

}

}