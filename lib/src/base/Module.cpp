#include <general/predefs.h>

#include <base/Module.h>
#include <utils/converters.h>

#include <numeric>

namespace nr
{

namespace __internal
{

string finalizeOptions(const Module::Options& options)
{
    string ret = "";
    for (auto it = options.cbegin(); it != options.cend(); ++it)
    {
        ret += " " + it->getOption();
    }

    return ret;    
}

Module::Module(const string& code, const Module::Options& options, cl_int* err)
{
    cl_int error = CL_SUCCESS;
    
    m_module = cl::Program(code, &error);
    if (error != CL_SUCCESS) 
    {
        if (err != nullptr) *err = error;
        return;
    }

    string soptions = finalizeOptions(options);
    m_module.build(soptions.c_str());
}

Module::Module(const std::initializer_list<string> codes, const Module::Options& options, cl_int* err)
{
    cl_int error = CL_SUCCESS;
    
    m_module = cl::Program(codes, err);
    if (error != CL_SUCCESS) 
    {
        if (err != nullptr) *err = error;
        return;
    }
    
    m_module.build(finalizeOptions(options).c_str());
}

Module::Macro DEBUG = Module::Macro("_DEBUG");

Module::CLVersion CL_VERSION_20 = Module::CLVersion(2.0);
Module::CLVersion CL_VERSION_12 = Module::CLVersion(1.2);

Module::Option WARNINGS_ARE_ERRORS = Module::Option("-Werror");

Module::RenderDimension _3D = Module::RenderDimension(3);
Module::RenderDimension _4D = Module::RenderDimension(4);
Module::RenderDimension _5D = Module::RenderDimension(5);

}

}
