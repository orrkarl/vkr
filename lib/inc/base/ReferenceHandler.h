/**
 * @file ReferenceHandler.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Reference count handling for basic OpenCL types (e.g. cl_command_queue)
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

/**
 * @brief Template supertype for reference handlers, to be specialized for each OpenCL basic type. 
 * Should provide "release" and "retain", 2 static methods which handle the type's reference count
 * 
 * @tparam cl_type The handled OpenCL type
 */
template<typename cl_type>
class ReferenceHandler { };


/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_device_id>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the device's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_device_id obj) { return clRetainDevice(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the device's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_device_id obj) { return clReleaseDevice(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_sampler>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the sampler's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_sampler obj) { return clRetainSampler(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the sampler's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_sampler obj) { return clReleaseSampler(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_command_queue>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the commandQueue's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_command_queue obj) { return clRetainCommandQueue(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the commandQueue's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_command_queue obj) {return clReleaseCommandQueue(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_context>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the context's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_context obj) { return clRetainContext(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the context's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_context obj) { return clReleaseContext(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_event>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the programs reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_event obj) { return clRetainEvent(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the programs refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_event obj) { return clReleaseEvent(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_mem>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the memObject's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_mem obj) { return clRetainMemObject(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the memObject's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_mem obj) { return clReleaseMemObject(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_kernel>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the kernel's reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_kernel obj) { return clRetainKernel(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the kernel's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_kernel obj) { return clReleaseKernel(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */                                                                     
template<>                                                              
class ReferenceHandler<cl_program>                                               
{                                                                       
public:                                                                 
    /**                                                                            
     * @brief Increments the programs reference count                              
     * @param obj The OpenCL object                                                
     */                                                                            
    static cl_status retain(cl_program obj) { return clRetainProgram(obj); }      
                                                                        
    /**                                                                 
     * @brief Decrements the programs refernece count                   
     * @param obj The OpenCL object                                                
     */                                                                 
    static cl_status release(cl_program obj) { return clReleaseProgram(obj); }    
};

/**                                                                     
 * @brief Uniform access to OpenCL types reference counting             
 *                                                                      
 */ 
template<>                                                      
class ReferenceHandler<cl_platform_id>                                     
{                                                               
public:     
    /**                                                                 
     * @brief Increments the object's refernece count                   
     * @param obj The OpenCL object                                                
     */                                                       
    static cl_status retain(cl_platform_id obj) { return CL_SUCCESS; }  

    /**                                                                 
     * @brief Decrements the object's refernece count                   
     * @param obj The OpenCL object                                                
     */   
    static cl_status release(cl_platform_id obj) { return CL_SUCCESS; }   
};
