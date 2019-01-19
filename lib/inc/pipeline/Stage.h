#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"

namespace nr 
{

namespace __internal
{

class Stage
{
public:

    virtual ~Stage()
    {
        
    }

    string getCompilationLog() const
    {
        return code.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault());
    }
    
protected:
    cl::Program code;

    Stage(const string& stageFile, cl_int& err);

    static const char* STANDARD_COMPILE_OPTIONS;
};

}

}
