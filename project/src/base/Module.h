/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief main interface for interacting with device code
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../predefs.h"

#include <vector>

#include "Context.h"
#include "Device.h"
#include "Kernel.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Host representation of a device code compilation unit
 * 
 * A module is a piece of OpenCL device code (in clc), which can be built and used to create kernels
 * @note wraps cl_program 
 */
class NRAPI Module : public Wrapper<cl_program>
{
public:
	/**
	 * @brief An OpenCL compiler option. 
	 * 
	 * This class represents the raw option. 
	 * It doesn't know if the option is a macro, an optimization level or some vendor-specific directive, so the entire option has to be specified.
	 * For example: passing `"FOO=1"` will result in an `CL_UNKNOWN_COMPILER_ARGS` error; pass `"-D FOO=1"` (or use Macro) instead.
	 * 
	 */
	struct NRAPI Option
	{
	public:
		/**
		 * @brief Constructs a raw OpenCL compiler option, which may be any string
		 * 
		 * @param value raw option string
		 */
		Option(const string& value);
	
		/**
		 * @brief aquire the stored option
		 * 
		 * @return const string& raw option value
		 */
		const string& getOption() const;

	protected:
		const string value;
	};

	typedef std::vector<Option> Options;

	/**
	 * @brief specifies a OpenCL compiler Macro
	 * 
	 */
	struct NRAPI Macro : Option
	{
		/**
		 * @brief Constructs a defined macro with no value
		 * 
		 * This class passes a macro to the compiler, which gets the equivelant effect of using `#define`. 
		 * For example, `Macro("FOO")` has the same effect as `#define FOO` in the device code
		 * @param name macro definition name
		 */
		Macro(const string name);
	
		/**
		 * @brief Constructs a defined macro with a value
		 * 
		 * This class passes a macro to the compiler, which gets the equivelant effect of using `#define`. 
		 * For example, `Macro("FOO", "3")` has the same effect as `#define FOO 3` in the device code
		 * @param name macro definition name
		 * @param value macro definition value
		 */
		Macro(const string name, const string value);
	};

	/**
	 * @brief Wraps the "-clstd" OpenCL option
	 * 
	 * Use this class to fix the OpenCL device code version
	 */
	struct NRAPI CLVersion : Option
	{
		/**
		 * @brief Construct a CLVersion object
		 * 
		 * The version is expected to be a string which matches an available OpenCL code version, e.g "1.2" or "2.0".
		 * Invalid version strings will only report errors at module build time.
		 * @param version version string
		 */
		CLVersion(const string& version);
	};

	typedef std::vector<string> Sources;
	typedef std::vector<Device> Devices;

	/**
	 * @brief Constructs a null module, with no device code attached to it
	 * 
	 */
    Module();

	/**
	 * @brief Constructs a modul object a single source, bound to a given context
	 * 
	 * @note wraps clCreateProgramWithSource
	 * @note refer to the OpenCL documentation for detailed explenation of call status
	 * @param context parent context
	 * @param code raw OpenCL device code
     * @param[out] err internal OpenCL call status
	 */
	Module(const Context& context, const string& code, cl_status& err);

	/**
	 * @brief Constructs a modul object a multiple sources, bound to a given context
	 * 
	 * @note wraps clCreateProgramWithSource
	 * @note refer to the OpenCL documentation for detailed explenation of call status
	 * @param context parent context
	 * @param codes raw OpenCL device codes
     * @param[out] err internal OpenCL call status
	 */
	Module(const Context& context, const Sources& codes, cl_status& err);

	/**
	 * @brief Convertes a raw OpenCL program to C++ wrapper
	 * 
     * This method allowes the class to "take ownership" of the lower OpenCL type; It may retain (increase the reference count) of the object
	 * @param module object to own
	 * @param retain should the reference count for the object be incremented
	 */
    Module(const cl_program& module, const nr_bool retain, cl_status& status);

    Module(const Module& other);

    Module(Module&& other);

    Module& operator=(const Module& other);

    Module& operator=(Module&& other);

	operator cl_program() const;

	cl_program get() const;

	/**
	 * @brief Builds the module for a single device
	 * 
	 * Compiles and links the entire module for the given device with given compiler options.
	 * The build action here is blocking, meaning that the function will block until build is completed.
	 * @note wraps clBuildProgram
	 * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and status codes
	 * @param device module build target
	 * @param options compilation options
	 * @return internal OpenCL call status
	 */
	cl_status build(const Device& device, const Options& options);

	/**
	 * @brief Builds the module for a multiple devices
	 * 
	 * Compiles and links the entire module for the given devices with given compiler options
	 * The build action here is blocking, meaning that the function will block until build is completed.
	 * @note wraps clBuildProgram
	 * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and status codes
	 * @param devices module build targets
	 * @param options compilation options
	 * @return internal OpenCL call status
	 */
	cl_status build(const Devices& devices, const Options& options);

	/**
	 * @brief Builds the module for a given device asynchronously
	 * 
	 * Compiles and links the entire module for the given device with given compiler options
	 * The build action here is non-blocking, meaning that the function should return immediatly and notify compilation status with the callback.
	 * @note wraps clBuildProgram
	 * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and status codes
	 * @tparam T user data type
	 * @param device module build target
	 * @param options compilation options 
     * @param callback context message callback
     * @param userData data to be passed to the context each time it's called
	 * @return internal OpenCL call status
	 */
	template<typename T>
	cl_status build(const Device& device, const Options& options, std::function<void(Module, T*)> callback, T* userData)
	{
		return clBuildProgram(
			object, 
			1, &device, 
			Module::finalizeOptions(options).c_str(), 
			static_cast<void(CL_CALLBACK *)(cl_program, void*)>([callback](cl_program prog, void* data){callback(Module(prog, true), reinterpret_cast<T*>(data));}), 
			userData);
	}
	
	/**
	 * @brief Builds the module for a given devices asynchronously
	 * 
	 * Compiles and links the entire module for the given devices with given compiler options
	 * The build action here is non-blocking, meaning that the function should return immediatly and notify compilation status with the callback.
	 * @note wraps clBuildProgram
	 * @note refer to OpenCL clBuildProgram documentation for detailed explenation of compiler options and status codes
	 * @tparam T user data type
	 * @param devices module build targets
	 * @param options compilation options 
     * @param callback context message callback
     * @param userData data to be passed to the context each time it's called
	 * @return internal OpenCL call status
	 */
	template<typename T>
	cl_status build(const Devices& devices, const Options& options, void(*callback)(Module, T*), T* userData)
	{
		return clBuildProgram(
			object, 
			devices.size(), &devices[0], 
			Module::finalizeOptions(options).c_str(), 
			static_cast<void(CL_CALLBACK *)(cl_program, void*)>([callback](cl_program prog, void* data){callback(Module(prog, true), reinterpret_cast<T*>(data));}), 
			userData);
	}

	/**
	 * @brief Create a Kernel object from this module
	 * 
	 * @param name kernel name
	 * @param[out] err internal OpenCL call status
	 * @return Kernel generated kernel object
	 */
	Kernel createKernel(const string& name, cl_status& err) const;

	/**
	 * @brief Create a Kernel object from this module
	 * 
	 * @param name kernel name
	 * @param[out] err internal OpenCL call status
	 * @return Kernel generated kernel object
	 */
	Kernel createKernel(const char* name, cl_status& err) const;

	/**
	 * @brief Get the Build Log object
	 * 
	 * After a build was called, a build log is generated. If the build was unsuccessful, 
	 * a log explaining the problem in a human readable format will be generated
	 * @param device target build device
	 * @param[out] err internal OpenCL call status
	 * @return string 
	 */
	string getBuildLog(Device device, cl_status& err) const;

	string getKernelNames(cl_status& err) const;

private:
	/**
	 * @brief finalizes a list of options into a single string which OpenCL can handle
	 * 
	 * @param  options the compiler options
	 * @return string  unified representation of passed options
	 */
	static string finalizeOptions(const Options& options);

public:
	static const Macro DEBUG;

	static const CLVersion CL_VERSION_22;
	static const CLVersion CL_VERSION_21;
	static const CLVersion CL_VERSION_20;
	static const CLVersion CL_VERSION_12;
	static const CLVersion CL_VERSION_11;
};

}
