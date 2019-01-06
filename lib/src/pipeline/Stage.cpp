#include <pipeline/Stage.h>

namespace nr
{

Stage::Stage(const string& fileContents, const string& stageName, const NRuint dimension, Error& err)
    : dimension(dimension)
{
    cl_int error;

    cl::Program code(fileContents, CL_TRUE, &error);
    if (error != CL_SUCCESS)
    {
        err = utils::fromCLError(error);

        if (err == Error::COMPILATION_FAILURE)
        {
            log = code.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault());
        }

        fprintf(stderr, "At program creation: %d\n", (NRint) err);
        fprintf(stderr, "%s\n", log.c_str());
        return;
    }

    kernel = cl::Kernel(code, stageName.c_str(), &error);
    if (error != CL_SUCCESS)
    {
        err = utils::fromCLError(error);
        fprintf(stderr, "At kernel creation: %d\n", (NRint) err);
        return;
    }
}

}

