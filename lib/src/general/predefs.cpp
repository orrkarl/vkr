#include <general/predefs.h>

namespace nr
{

namespace error
{
	nr_bool isSuccess(const cl_status& err) { return err == CL_SUCCESS; }
	nr_bool isFailure(const cl_status& err) { return err != CL_SUCCESS; }
}


}
