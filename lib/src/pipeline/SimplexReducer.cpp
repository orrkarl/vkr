#include <pipeline/SimplexReducer.h>

namespace nr
{

namespace detail
{

nr_uint SimplexReducer::getTriangleCount(const nr_uint simplexCount)
{
	return simplexCount * (simplexCount - 1) * (simplexCount - 2) / 6;
}

SimplexReducer::SimplexReducer(Module module, cl_status* err)
	: Kernel(module, "reduce_simplex", err)
{
}

SimplexReducer::SimplexReducer()
	: Kernel()
{
}

cl_status SimplexReducer::load()
{
	cl_status err = CL_SUCCESS;
	if ((err = setArg(0, simplexes)) != CL_SUCCESS) return err;
	return setArg(1, result);
}

}

}