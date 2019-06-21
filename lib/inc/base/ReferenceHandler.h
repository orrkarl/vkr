#pragma once

#include "../general/predefs.h"

template<typename cl_type>
class ReferenceHandler { };

#define ReferenceHandler(T, Name)                                           \
    template<>                                                              \
    class ReferenceHandler<T>                                               \
    {                                                                       \
    public:                                                                 \
        static cl_status retain(T obj) { return clRetain##Name(obj); }      \
        static cl_status release(T obj) { return clRelease##Name(obj); }    \
    };

ReferenceHandler(cl_program, Program)
ReferenceHandler(cl_kernel, Kernel)
ReferenceHandler(cl_mem, MemObject)
ReferenceHandler(cl_event, Event)
ReferenceHandler(cl_context, Context)
ReferenceHandler(cl_command_queue, CommandQueue)
ReferenceHandler(cl_sampler, Sampler)
ReferenceHandler(cl_device_id, Device)

template<>                                                      
class ReferenceHandler<cl_platform_id>                                     
{                                                               
public:                                                         
    static cl_status retain(cl_platform_id obj) { return CL_SUCCESS; }     
    static cl_status release(cl_platform_id obj) { return CL_SUCCESS; }   
};
