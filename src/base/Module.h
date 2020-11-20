#pragma once

#include "../predefs.h"

#include <vector>

#include "Context.h"
#include "Device.h"
#include "Kernel.h"
#include "Wrapper.h"

namespace nr::base {

CL_TYPE_CREATE_EXCEPTION(Module, CLApiException);

class BuildFailedException : public CLApiException {
public:
    explicit BuildFailedException(Status status);
};

/**
 * @brief Host representation of a device code compilation unit
 *
 * A module is a piece of OpenCL device code (in clc), which can be built and used to create kernels
 * @note wraps cl_program
 */
class Module {
public:
    using Sources = std::vector<std::string>;
    using Devices = std::vector<DeviceView>;

    /**
     * @brief Constructs a module object a single source, bound to a given context
     *
     * @note wraps clCreateProgramWithSource
     * @note refer to the OpenCL documentation for detailed explenation of call status
     * @param context parent context
     * @param code raw OpenCL device code
     */
    Module(Context& context, const std::string& code);

    /**
     * @brief Constructs a module object a multiple sources, bound to a given context
     *
     * @note wraps clCreateProgramWithSource
     * @note refer to the OpenCL documentation for detailed explenation of call status
     * @param context parent context
     * @param codes raw OpenCL device codes
     */
    Module(Context& context, const Sources& codes);

    cl_program rawHandle();

    /**
     * @brief Builds the module for a single device
     *
     * Compiles and links the entire module for the given device with given compiler options.
     * The build action here is blocking, meaning that the function will block until build is completed.
     * @note wraps clBuildProgram
     * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and
     * status codes
     * @param device module build target
     * @param options compilation options
     * @return internal OpenCL call status
     */
    void build(Device& device, const char* options);

    /**
     * @brief Builds the module for a multiple devices
     *
     * Compiles and links the entire module for the given devices with given compiler options
     * The build action here is blocking, meaning that the function will block until build is completed.
     * @note wraps clBuildProgram
     * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and
     * status codes
     * @param devices module build targets
     * @param options compilation options
     */
    void build(Devices& devices, const char* options);

    /**
     * @brief Get the Build Log object
     *
     * After a build was called, a build log is generated. If the build was unsuccessful,
     * a log explaining the problem in a human readable format will be generated
     *
     * @param device target build device
     * @return string
     */
    std::string getBuildLog(Device& device) const;

    std::vector<std::string> getKernelNames() const;

private:
    struct ProgramTraits {
        using Type = cl_program;
        static constexpr auto release = clReleaseProgram;
    };

    UniqueWrapper<ProgramTraits> m_object;
};

}
