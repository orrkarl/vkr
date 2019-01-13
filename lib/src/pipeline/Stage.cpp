#include <pipeline/Stage.h>

namespace nr
{

namespace __internal
{

Stage::Stage(const string& stageCode, cl_int& err)
{
    cl_int error;
    code = cl::Program(stageCode, CL_TRUE, &error);
}

}

}

