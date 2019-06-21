#pragma once

#include "../general/predefs.h"
#include "ReferenceHandler.h"

namespace nr
{

namespace __internal
{

template<typename cl_type>
class NR_SHARED Wrapper
{
// Methods and Constructors
public:
    Wrapper() 
        : object(NULL)
    {
    }

    Wrapper(const Wrapper<cl_type>& other)
        : object(other.object)
    {
        retain();
    }

    Wrapper(Wrapper<cl_type>&& other)
    {
        object = other.object;
        other.object = NULL;
    }

    Wrapper<cl_type>& operator=(const Wrapper<cl_type>& other)
    {
        if (this != &other)
        {
            release();
            object = other.object;
            retain();
        }

        return *this;
    }

    Wrapper<cl_type>& operator=(Wrapper<cl_type>&& other)
    {
        if (this != &other)
        {
            release();
            object = other.object;
            other.object = NULL;
        }

        return *this;
    }

    const cl_type& get() const 
    {
        return object;
    }

    cl_type& get()
    {
        return object;
    }

    operator cl_type() const
    {
        return object;
    }

protected:
    cl_status release() 
    { 
        cl_status ret = object == NULL ? ReferenceHandler<cl_type>::release(object) : CL_SUCCESS; 
        object = NULL;
        return ret;
    }

    cl_status retain() 
    { 
        return ReferenceHandler<cl_type>::retain(object); 
    }

// Fields
protected:
    cl_type object;
};

}

}