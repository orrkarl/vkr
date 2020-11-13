#pragma once

#include "../predefs.h"

#include <functional>
#include <vector>

#include "DeviceType.h"
#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

class Device;

CL_TYPE_CREATE_EXCEPTION(Context);

/**
 * @brief bare wrapper for cl_context, for a more comfortable interface.
 *
 * The OpenCL context, behind the stages, is a container for many of the basic OpenCL components, such as
 * buffers or programs.
 */
class Context {
public:
    /**
     * @brief Construct a context object, bound to some devices with certain properties
     *
     * Refer to the OpenCL clCreateContext documentation for detailed explanation of the context properties
     * @note wraps clCreateContext
     * @note if devices has more than MAX_UINT values, this function will throw with status CL_INVALID_VALUE
     * @param properties context properties
     * @param devices devices bound to the context
     */
    Context(const cl_context_properties* properties, std::vector<Device>& devices);

    /**
     * @brief an 'easy to use' constructor for a Context. Using the given properties and collects all of the
     * Devices of a certain type
     *
     * Refer to the OpenCL clCreateContext documentation for detailed explanation of the context properties
     * @note wraps clCreateContextFromType
     * @param       properties  context properties
     * @param       deviceType  type of collected devices. Will default to GPU.
     */
    explicit Context(
        const cl_context_properties* properties, DeviceTypeBitField deviceType = DeviceTypeFlag::GPU);

    /**
     * @brief provides access to the underlying OpenCL context
     *
     * @return cl_context underlying context
     */
    [[nodiscard]] cl_context rawHandle();

private:
    struct ContextTraits {
        using Type = cl_context;
        static constexpr auto release = clReleaseContext;
    };

    UniqueWrapper<ContextTraits> m_context;
};

}