#pragma once

#include "../predefs.h"

#include "../kernels/simplex_reducer.cl.h"
#include "../utils/StandardDispatch.h"

namespace nr
{

namespace detail
{

class SimplexReducer : public StandardDispatch<1, Buffer, Buffer>
{
public:
	SimplexReducer(const Module& module, cl_status* err = nullptr)
		: StandardDispatch(module, SIMPLEX_REDUCE_KERNEL_NAME, err)
	{
	}

	SimplexReducer()
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

}

}