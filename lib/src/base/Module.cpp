#include <general/predefs.h>

#include <base/Module.h>
#include <utils/converters.h>
#include <utils/files.h>


namespace nr
{

namespace __internal
{

Module::Module(const string& code, const char* options, cl_int* err)
{
    cl_int error = CL_SUCCESS;
    
    m_module = cl::Program(code, &error);
    if (error != CL_SUCCESS) 
    {
        if (err != nullptr) *err = error;
        return;
    }

    m_module.build(options);
}

Module::Module(const std::vector<string> codes, const char* options, cl_int* err)
{
    cl_int error = CL_SUCCESS;
    
    m_module = cl::Program(codes, err);
    if (error != CL_SUCCESS) 
    {
        if (err != nullptr) *err = error;
        return;
    }
    
    m_module.build(options);
}


cl_int Module::make(const string& fileName, const char* options, Module& module)
{
    cl_int ret = CL_SUCCESS; 
    auto code = utils::loadFile(fileName);
    module = Module(code, options, &ret);
    return ret;
}

cl_int Module::make(const std::vector<string> files, const char* options, Module& module)
{
    cl_int ret = CL_SUCCESS; 
    
    auto codes = std::vector<string>(files.size());
    for (NRuint i = 0; i < codes.size(); ++i)
    {
        codes[i] = utils::loadFile(files[i]); 
    }

    module = Module(codes, options, &ret);
    return ret;
}

}

}