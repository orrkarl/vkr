#include <pipeline/VertexBuffer.h>

#include <utils/rendermath.h>

namespace nr
{
VertexBuffer::VertexBuffer(const Context& context, const nr_uint dimension, const nr_uint primitiveCount, const nr_float* data, cl_status* err)
	:	vertecies(context, CL_MEM_READ_WRITE, (dimension + 1)* primitiveCount * sizeof(nr_float), err), 
		reducedVertecies(context, CL_MEM_READ_WRITE, (dimension + 1)* primitiveCount * sizeof(nr_float), err),
		reducedSimplices(context, CL_MEM_READ_WRITE, (dimension + 1)* detail::triangleCount(dimension, primitiveCount) * sizeof(nr_float), err);
{
}

VertexBuffer::operator Buffer() const
{
	return vertecies;
}

}

