/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief provides a C++ style wrapper for cl_device
 * @version 0.6.0
 * @date 2019-08-26
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include "Wrapper.h"

namespace nr
{

/**
 * @brief Represents an OpenCL physical device, such as a specific GPU exsiting in the computer.
 * 
 * This class is mostly used for configuration and initialization
 */
class NRAPI Device : public Wrapper<cl_device_id>
{
public:
    Device();

    /**
	 * @brief Convertes a raw OpenCL device to the C++ wrapper
	 * 
	 * @param device object to own
	 * @param retain should the reference count for the object be incremented
	 */
    explicit Device(const cl_device_id& device, const nr_bool retain = false);

    Device(const Device& other);

    Device(Device&& other);

    Device& operator=(const Device& other);

    Device& operator=(Device&& other);

    /**
	 * @brief access the underlying opencl device
	 * 
	 * @return cl_device_id raw opencl device
	 */
    operator cl_device_id() const;

    /**
	 * @brief access the underlying opencl device
	 * 
	 * @return const cl_device_id& raw opencl device
	 */
    const cl_device_id& get() const;

    /**
     * @brief Queries for the device name
     * 
     * @return string the device name
     */
	string name() const;
};

}