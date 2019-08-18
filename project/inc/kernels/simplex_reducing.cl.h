#pragma once

#include "../general/predefs.h"

#include "../utils/StandardDispatch.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* SIMPLEX_REDUCE_KERNEL_NAME;

class SimplexReduceKernel : public StandardDispatch<1, Buffer, Buffer>
{
public:
	SimplexReduceKernel(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, SIMPLEX_REDUCE_KERNEL_NAME, err)
	{
	}

	SimplexReduceKernel()
		: StandardDispatch()
	{
	}

	cl_status setSimplexInputBuffer(const Buffer& in)
	{
		return setArg<INPUT_BUFFER>(in);
	}

	cl_status setTriangleOutputBuffer(const Buffer& out)
	{
		return setArg<OUTPUT_BUFFER>(out);
	}

	void setExecutionRange(const nr_uint primitiveCount)
	{
		range.global.x = primitiveCount;
		range.local.x = 1;
	}
	
	using TypesafeKernel<Buffer, Buffer>::operator cl_kernel;

private:
	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint OUTPUT_BUFFER = 1;

};

namespace clcode
{

// Reducing simplexes to their triangles
NRAPI extern const char* simplex_reducing;

}

}

}
