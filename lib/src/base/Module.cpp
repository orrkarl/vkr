#include <general/predefs.h>

#include <base/Module.h>
#include <utils/converters.h>

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

Module::Module(const std::initializer_list<string> codes, const char* options, cl_int* err)
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

}

}