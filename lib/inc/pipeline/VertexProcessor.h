#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"

#include "../kernels/Source.h"

#include "Stage.h"

namespace nr
{

namespace detail
{

namespace vertex_processor
{

struct Params
{
	Buffer near;
	Buffer far;
};

struct Inputs
{
	Buffer simplexes;
	nr_uint simplexCount;
};

struct Outputs
{
	Buffer result;
};

}

class NRAPI VertexProcessor : public Dispatch<VertexProcessor>, public Stage<vertex_processor::Params, vertex_processor::Inputs, vertex_processor::Outputs>
{
public:
	cl_status setRenderDimension(const Source& source);

	cl_status setNearPlane(const CommandQueue& q, const nr_float* near);

	cl_status setFarPlane(const CommandQueue& q, const nr_float* far);

	cl_status consume(const CommandQueue& q);

private:
	SimplexReduceKernel m_simplexReduce;
	NDExecutionRange<1> m_simplexReduceRange{ 0, 0 };

	VertexReduceKernel  m_vertexReduce;
	NDExecutionRange<1> m_vertexReduceRange{ 0, 0 };

	nr_uint m_currentRenderDimension = 0;
};

}

}