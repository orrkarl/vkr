#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"

namespace nr 
{

class Stage
{
public:

    virtual ~Stage()
    {
        
    }

    string getCompilationLog() const
    {
        return log;
    }
    
protected:
    const NRuint dimension;

    cl::Kernel kernel;

    Stage(const string& fileContents, const string& stageName, const NRuint dimension, Error& err);

private:
    string log;
};

}
