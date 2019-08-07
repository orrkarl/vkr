#include <kernels/simplex_reducing.cl.h>

namespace nr
{

namespace detail
{

extern const char* SIMPLEX_REDUCE_KERNEL_NAME = "reduce_simplex";

SimplexReduceKernel::SimplexReduceKernel(const Module& module, cl_status* err)
	: TypesafeKernel<Buffer, Buffer>(module, SIMPLEX_REDUCE_KERNEL_NAME, err)
{
}

SimplexReduceKernel::SimplexReduceKernel()
	: TypesafeKernel()
{
}

cl_status SimplexReduceKernel::consume(const CommandQueue& q)
{
	return q.enqueueKernelCommand<1>(*this, m_range);
}

cl_status SimplexReduceKernel::setSimplexInputBuffer(const Buffer& in)
{
	return setArg<INPUT_BUFFER>(in);
}

cl_status SimplexReduceKernel::setTriangleOutputBuffer(const Buffer& out)
{
	return setArg<OUTPUT_BUFFER>(out);
}

void SimplexReduceKernel::setExecutionRange(const nr_uint primitiveCount)
{
	m_range.global.x = primitiveCount;
	m_range.local.x = 1;
}

NDExecutionRange<1> SimplexReduceKernel::getExecutionRange() const
{
	return m_range;
}

namespace clcode
{

// Reducing simplexes to their triangles
extern const char* simplex_reducing = R"__CODE__(

#define TRIANGLES_PER_SIMPLEX (RENDER_DIMENSION * (RENDER_DIMENSION - 1) * (RENDER_DIMENSION - 2) / 6) // There are nC3 triangles in an n-1 simplex

void copy_point(const global Point p, global Point res)
{
	for (uint i = 0; i < ELEMENTS_PER_POINT; ++i)
	{
		res[i] = p[i];
	}
}

void emit_triangle(const global Point p0, const global Point p1, const global Point p2, global Triangle result)
{
	copy_point(p0, result[0]);
	copy_point(p1, result[1]);
	copy_point(p2, result[2]);
}

kernel void reduce_simplex(
	const global Simplex* simplexes, 
	global Triangle* result)
{
	const uint index = get_global_id(0);
	uint res = get_global_id(0) * TRIANGLES_PER_SIMPLEX;
	
	for (uint i = 0; i < RENDER_DIMENSION - 2; ++i)
	{
		for (uint j = i + 1; j < RENDER_DIMENSION - 1; ++j)
		{
			for (uint k = j + 1; k < RENDER_DIMENSION; ++k)
			{
				emit_triangle(simplexes[index][i], simplexes[index][j], simplexes[index][k], result[res++]);
			}
		}	
	}
}

)__CODE__";

}

}

}
