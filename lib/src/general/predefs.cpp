#include <general/predefs.h>

namespace nr
{

namespace error
{
	NRbool isSuccess(const cl_status& err) { return err == CL_SUCCESS; }
	NRbool isFailure(const cl_status& err) { return err != CL_SUCCESS; }
}


}
