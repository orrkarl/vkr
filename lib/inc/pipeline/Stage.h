#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"

namespace nr 
{

class Stage
{
public:

    virtual Error apply(const cl::Buffer& src, const cl::Buffer& dest, const cl::CommandQueue& queue) = 0;

    virtual ~Stage()
    {
        
    }
    
protected:
    const NRuint dimension;

    Stage(const string& fileContents, const string& stageName, const NRuint dimension, Error& err);

    cl::Kernel* getKernel()
    {
        return &kernel;
    }

private:
    cl::Kernel kernel;
    const string log;
};

}
