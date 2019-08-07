#include <pipeline/VertexShader.h>

namespace nr
{

namespace detail
{

VertexShader::VertexShader(Module module, cl_status* err)
	: Kernel(module, "shade_vertex", err)
{
}

VertexShader::VertexShader()
	: Kernel()
{
}

cl_status VertexShader::load()
{
	cl_status err = CL_SUCCESS;

	if ((err = setArg(0, points)) != CL_SUCCESS) return err;
	if ((err = setArg(1, near)) != CL_SUCCESS) return err;
	if ((err = setArg(2, far)) != CL_SUCCESS) return err;
	return setArg(3, result);
}

}

}