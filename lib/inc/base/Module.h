#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"
#include "Kernel.h"

namespace nr
{

namespace __internal
{

class NR_SHARED_EXPORT Module
{

public:
    Module() {}

    Module(const string& code, const char* options, cl_int* err);
    Module(const std::initializer_list<string> codes, const char* options, cl_int* err);

    template<class Params>
    Kernel<Params> makeKernel(const string& name, cl_int* err)
    {
        return Kernel<Params>(m_module, name.data(), err);
    }

    string getBuildLog(cl_int* err) const
    {
        auto ret = m_module.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault(), err);
        return ret;
    }

private:
    cl::Program m_module;
};

}

}