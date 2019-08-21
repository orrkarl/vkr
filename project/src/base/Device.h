/**
 * @file Device.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping OpenCL cl_device_id 
 * @version 0.5.9
 * @date 2019-06-30
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
 */
class NRAPI Device : public Wrapper<cl_device_id>
{
public:
    Device();

    explicit Device(const cl_device_id& device, const nr_bool retain = false);

    Device(const Device& other);

    Device(Device&& other);

    Device& operator=(const Device& other);

    Device& operator=(Device&& other);

    operator cl_device_id() const;

    const cl_device_id& get() const;

	string name() const;
};

}