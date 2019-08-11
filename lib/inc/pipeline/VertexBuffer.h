#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"

#include "../utils/rendermath.h"

namespace nr
{

class VertexBuffer
{
public:
	template <typename T>
	VertexBuffer(const Context& context, const nr_uint dimension, const nr_uint primitiveCount, const T* data, cl_status* err)
		:	vertecies(context, CL_MEM_READ_WRITE, (dimension + 1) * primitiveCount * sizeof(T), err),
			reducedVertecies(context, CL_MEM_READ_WRITE, (dimension + 1) * primitiveCount * sizeof(T), err),
			reducedSimplices(context, CL_MEM_READ_WRITE, (dimension + 1) * detail::triangleCount(dimension, primitiveCount) * sizeof(T), err)
	{
	}

	operator Buffer() const { return vertecies; }

private:
	friend class Pipeline;

	const Buffer vertecies;
	const Buffer reducedVertecies;
	const Buffer reducedSimplices;
};

}