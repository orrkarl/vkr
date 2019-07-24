/**
 * @file Module.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Wrapping cl_program
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../general/predefs.h"

#include <vector>

#include "Context.h"
#include "Device.h"
#include "Kernel.h"
#include "Wrapper.h"

namespace nr
{

/**
 * @brief Wrapper class for cl_program, provides a more interface for compiler options
 * 
 */
class NRAPI Module : public Wrapper<cl_program>
{
public:
	/**
	 * @brief An OpenCL compiler option. 
	 * @par
	 * This class represents the entire option. 
	 * It doesn't know if the option is a macro or an optimization level, and as such the entire option has to be specified. For example: passing "FOO=1" 
	 * will result in an error; pass "-D FOO=1" instead.
	 * 
	 */
	struct NRAPI Option
	{
		public:
			Option(const string& value);
		
			string getOption() const;

		protected:
			const string value;
	};

	typedef std::vector<Option> Options;

	struct NRAPI Macro : Option
	{
		Macro(const string name);
	
		Macro(const string name, const string value);
	};

	struct NRAPI RenderDimension : Macro
	{
		RenderDimension(const nr_uint& dimension);
	};

	struct NRAPI CLVersion : Option
	{
		CLVersion(const string& version);
	};

	typedef std::vector<string> Sources;
	typedef std::vector<Device> Devices;

	
// Methods and Constructors
public:
    Module();

	template<nr_uint N>
	explicit Module(Context& context, const std::array<string, N>& codes, const std::array<nr_uint, N>& sizes, cl_status* err = nullptr)
		: Wrapped(clCreateProgramWithSource(context, N, codes.data(), sizes.data(), err))
	{
	}

	Module(Context context, const string& code, cl_status* err = nullptr);

	Module(Context context, const Sources& codes, cl_status* err = nullptr);

    explicit Module(const cl_program& module, const nr_bool retain = false);

    Module(const Module& other);

    Module(Module&& other);

    Module& operator=(const Module& other);

    Module& operator=(Module&& other);

	operator cl_program() const;

	cl_status build(const Device& device, const Options& options);

	cl_status build(const Devices& devices, const Options& options);

	template<typename T>
	cl_status build(const Device& device, const Options& options, std::function<void(Module, T*)> callback, T* userData)
	{
		return clBuildProgram(
			object, 
			1, &device, 
			Module::finalizeOptions(options).c_str(), 
			static_cast<void(CL_CALLBACK *)(cl_program, void*)>([callback](cl_program prog, void* data){callback(Module(prog, true), (T*) data);}), 
			userData);
	}
	
	template<typename T>
	cl_status build(const Options& options, std::function<void(Module, T*)> callback, T* userData)
	{
		auto dev = static_cast<cl_device_id>(Device::getDefault());
		return clBuildProgram(
			object, 
			1, &dev, 
			Module::finalizeOptions(options).c_str(), 
			static_cast<void(CL_CALLBACK *)(cl_program, void*)>([callback](cl_program prog, void* data){callback(Module(prog, true), (T*) data);}), 
			userData);
	}

	template<typename T>
	cl_status build(const Devices& devices, const Options& options, void(*callback)(Module, T*), T* userData)
	{
		return clBuildProgram(
			object, 
			devices.size(), &devices[0], 
			Module::finalizeOptions(options).c_str(), 
			static_cast<void(CL_CALLBACK *)(cl_program, void*)>([callback](cl_program prog, void* data){callback(Module(prog, true), (T*) data);}), 
			userData);
	}

	Kernel createKernel(const string& name, cl_status* err = nullptr);

	string getBuildLog(Device device, cl_status* err = nullptr);

private:
	/**
	 * @brief finalizes a list of options into a single string which OpenCL can handle
	 * 
	 * @param 	options 	the compiler options
	 * @return 	the string representation of passed options
	 * 
	 */
	static string finalizeOptions(const Options& options);

// Fields
public:
	static const Macro DEBUG;

	static const CLVersion CL_VERSION_22;
	static const CLVersion CL_VERSION_21;
	static const CLVersion CL_VERSION_20;
	static const CLVersion CL_VERSION_12;
	static const CLVersion CL_VERSION_11;

	static const RenderDimension _3D;
	static const RenderDimension _4D;
	static const RenderDimension _5D;
};

}
