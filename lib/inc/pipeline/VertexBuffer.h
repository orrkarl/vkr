#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"

namespace nr
{

class VertexBuffer
{
	VertexBuffer(const Context& context, const nr_uint dimension, const nr_uint primitiveCount, const nr_float* data, cl_status* err);

	operator Buffer() const;

private:
	friend class Pipeline;

	const Buffer vertecies;
	const Buffer reducedVertecies;
	const Buffer reducedSimplices;
};

}