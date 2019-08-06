#include "..\..\inc\pipeline\VertexProcessor.h"

namespace nr
{

namespace __internal
{

const string VertexProcessor::SIMPLEX_REDUCE_NAME = "reduce_simplex";
const string VertexProcessor::VERTEX_REDUCE_NAME = "shade_vertex";

VertexProcessor::VertexProcessor(const Module& code, const nr_uint dim)
	: m_simplexReduce(code, SIMPLEX_REDUCE_NAME, nullptr), m_vertexReduce(code, VERTEX_REDUCE_NAME, nullptr)
{
}

void VertexProcessor::setRenderDimension(const Module& code, const nr_uint dim)
{
	m_simplexReduce = Kernel(code, SIMPLEX_REDUCE_NAME, nullptr);
	m_vertexReduce = Kernel(code, VERTEX_REDUCE_NAME, nullptr);
}

void VertexProcessor::setNearPlane(nr::CommandQueue* q, const nr_float* near)
{

}

void VertexProcessor::setFarPlane(nr::CommandQueue* q, const nr_float* far)
{
}

cl_status VertexProcessor::consume(nr::CommandQueue* q)
{
	return cl_status();
}

}

}