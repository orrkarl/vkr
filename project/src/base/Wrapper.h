/**
 * @file Wrapper.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping OpenCL built in reference handling, implementing copy and move constructors and operators accordingly
 * @see ReferenceHandler.h
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include "../predefs.h"

#include "ReferenceHandler.h"

namespace nr
{

/**
 * @brief Wrapping class for basic OpenCL types, allowing to copy and move them while updating their referene count correctly 
 * 
 * This is a parent class, used for code sharing and aviding the need to copy the same operation time and time again.
 * @tparam cl_type underlying OpenCL type (e.g cl_device_id, cl_mem etc.)
 */
template<typename cl_type>
class Wrapper
{
public:
    /**
     * @brief Construct a null cl_type
     * 
     */
    Wrapper() 
        : object(nullptr)
    {
    }

    /**
     * @brief Allowes the C++ class to own a raw OpenCL object
     * 
     * @param object raw OpenCL object
     * @param retainObject should the object reference count be incremented
     */
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

    /**
     * @brief Decrement the underlying object's reference count by 1
     * 
     * @note refer to the OpenCL documentation for a detailed explenation of the object's reference count mechanism
     * @return cl_status internal OpenCL call status
     */
    cl_status release() 
    { 
        cl_status ret = object != nullptr ? ReferenceHandler<cl_type>::release(object) : CL_SUCCESS; 
        object = nullptr;
        return ret;
    }  
    
    /**
     * @brief Increment the underlying object's reference count
     * 
     * @note refer to the OpenCL documentation for a detailed explenation of the object's reference count mechanism
     * @return cl_status internal OpenCL call status
     */
    cl_status retain() 
    { 
        return object != nullptr ? ReferenceHandler<cl_type>::retain(object) : CL_SUCCESS; 
    }

protected:
    typedef Wrapper<cl_type> Wrapped;

    cl_type object;
};

}
