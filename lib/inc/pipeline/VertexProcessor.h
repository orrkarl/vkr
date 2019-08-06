#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "Stage.h"

namespace nr
{

namespace __internal
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

class VertexProcessor : public Dispatch<VertexProcessor>, public Stage<vertex_processor::Params, vertex_processor::Inputs, vertex_processor::Outputs>
{
public:
	VertexProcessor(const Module& code, const nr_uint dim);

	void setRenderDimension(const Module& code, const nr_uint dim);

	void setNearPlane(nr::CommandQueue* q, const nr_float* near);

	void setFarPlane(nr::CommandQueue* q, const nr_float* far);

	cl_status consume(nr::CommandQueue* q);

private:
	static const string SIMPLEX_REDUCE_NAME;
	static const string VERTEX_REDUCE_NAME;

	Kernel m_simplexReduce;
	NDExecutionRange<2> m_simplexReduceRange{ 0, 0 };

	Kernel m_vertexReduce;
	NDExecutionRange<2> m_vertexReduceRange{ 0, 0 };
};

}

}