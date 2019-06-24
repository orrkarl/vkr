#pragma once

#include "../general/predefs.h"
#include "ReferenceHandler.h"

namespace nr
{

template<typename cl_type>
class Wrapper
{
// Methods and Constructors
public:
    Wrapper() 
        : object(nullptr)
    {
    }

    explicit Wrapper(const cl_type object, const nr_bool retainObject = false)
        : object(object)
    {
        if (retainObject) retain();
    }

    ~Wrapper()
    {
        release();
    }

    Wrapper(const Wrapper<cl_type>& other)
        : object(other.object)
    {
        retain();
    }

    Wrapper(Wrapper<cl_type>&& other)
    {
        object = other.object;
        other.object = nullptr;
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
            other.object = nullptr;
        }

        return *this;
    }

protected:
    cl_status release() 
    { 
        cl_status ret = object == nullptr ? ReferenceHandler<cl_type>::release(object) : CL_SUCCESS; 
        object = nullptr;
        return ret;
    }

    cl_status retain() 
    { 
        return ReferenceHandler<cl_type>::retain(object); 
    }

// Fields
protected:
    cl_type object;

// Types
protected:
    typedef Wrapper<cl_type> Wrapped;
};

}
