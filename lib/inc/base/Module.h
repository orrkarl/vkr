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
 * Utility wrapper around cl_program
 **/
class NRAPI Module : public Wrapper<cl_program>
{
// Types
public:
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

	explicit Module(const string& code, cl_status* err = nullptr);

	explicit Module(const Sources& codes, cl_status* err = nullptr);

	template<nr_uint N>
	explicit Module(const std::array<nr_char*, N>& codes, const std::array<nr_uint, N>& sizes, cl_status* err = nullptr)
		: Module(Context::getDefault(), codes, sizes, err)
	{
	}

	template<nr_uint N>
	explicit Module(Context& context, const std::array<nr_char*, N>& codes, const std::array<nr_uint, N>& sizes, cl_status* err = nullptr)
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

	cl_status build(const Options& options);

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

	string getBuildLog(cl_status* err = nullptr);

	string getBuildLog(Device device, cl_status* err = nullptr);

private:
	static string finalizeOptions(const Options& options);

// Fields
public:
	static const Macro DEBUG;

	static const CLVersion CL_VERSION_22;
	static const CLVersion CL_VERSION_21;
	static const CLVersion CL_VERSION_20;
	static const CLVersion CL_VERSION_12;
	static const CLVersion CL_VERSION_11;

	static const Option WARNINGS_ARE_ERRORS;

	static const RenderDimension _3D;
	static const RenderDimension _4D;
	static const RenderDimension _5D;
};

}
