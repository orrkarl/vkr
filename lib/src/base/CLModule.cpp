#include <general/predefs.h>

#include <base/CLModule.h>
#include <utils/converters.h>
#include <utils/files.h>


namespace nr
{

namespace __internal
{

CLModule::CLModule(const string& code, cl_int* err)
{
    m_module = cl::Program(code, err);
    if (err != CL_SUCCESS) return;
    m_module.build(STANDARD_COMPILE_OPTIONS);
}

CLModule::CLModule(const std::vector<string> codes, cl_int* err)
{
    m_module = cl::Program(codes, err);
    m_module.build(STANDARD_COMPILE_OPTIONS);
}


cl_int CLModule::make(const string& fileName, CLModule& module)
{
    cl_int ret = CL_SUCCESS; 
    auto code = utils::loadFile(fileName);
    module = CLModule(code, &ret);
    return ret;
}

cl_int CLModule::make(const std::vector<string> files, CLModule& module)
{
    cl_int ret = CL_SUCCESS; 
    
    auto codes = std::vector<string>(files.size());
    for (NRuint i = 0; i < codes.size(); ++i)
    {
        codes[i] = utils::loadFile(files[i]); 
    }

    module = CLModule(codes, &ret);
    return ret;
}

}

}