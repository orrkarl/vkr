#include <pipeline/Stage.h>

namespace nr
{

namespace __internal
{

#ifndef NDEBUG
const char* Stage::STANDARD_COMPILE_OPTIONS = "-D _DEBUG";
#else
const char* Stage::STANDARD_COMPILE_OPTIONS = "";
#endif // _DEBUG

Stage::Stage(const string& stageCode, cl_int& err)
{
    cl_int error;
    code = cl::Program(stageCode, CL_FALSE, &error);
    code.build(Stage::STANDARD_COMPILE_OPTIONS);
}

}

}

