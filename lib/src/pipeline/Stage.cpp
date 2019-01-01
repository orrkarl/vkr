#include <pipeline/Stage.h>

namespace nr
{

Stage::Stage(const string& fileContents, const string& stageName, const NRuint dimension, Error& err)
    : dimension(dimension)
{
    cl_int error;

    cl::Program code({ fileContents }, &error);
    if (error != CL_SUCCESS)
    {
        err = utils::fromCLError(error);
        return;
    }

    kernel = cl::Kernel(code, stageName.c_str(), &error);
    if (error != CL_SUCCESS)
    {
        err = utils::fromCLError(error);
        return;
    }
}

}

