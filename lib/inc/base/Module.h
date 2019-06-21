#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"
#include "Wrapper.h"
#include "Kernel.h"
#include "Device.h"
#include "Context.h"
#include <vector>

namespace nr
{

/**
 * Utility wrapper around cl::Module
 **/
class NR_SHARED Module : public Wrapper<cl_program>
{
// Types
public:
	struct NR_SHARED Option
	{
		public:
			Option(const string& value)
				: value(value)
			{
			}
		
			string getOption() const { return value; }

		protected:
			const string value;
	};

	typedef std::vector<Option> Options;

	struct NR_SHARED Macro : Option
	{
		Macro(const string name)
			: Option("-D " + name)
		{
		}
	
		Macro(const string name, const string value)
			: Option("-D " + name + "=" + value)
		{
		}
	};

	struct NR_SHARED RenderDimension : Macro
	{
		RenderDimension(const NRuint& dimension)
			: Macro("RENDER_DIMENSION", std::to_string(dimension))
		{
		}
	};

	struct NR_SHARED CLVersion : Option
	{
		CLVersion(const string& version)
			: Option("-cl-std=CL" + version)
		{
		}
	};

	typedef std::vector<string> Sources;
	typedef std::vector<Device> Devices;

	
// Methods and Constructors
public:

    Module()
        : Wrapped()
    {
    }

	explicit Module(const string& code, cl_status* err = nullptr)
		: Module(Context::getDefault(), code, err)
	{
	}

	explicit Module(const Sources& codes, cl_status* err = nullptr)
		: Module(Context::getDefault(), codes, err)
	{
	}

	template<NRuint N>
	explicit Module(const std::array<NRchar*, N>& codes, const std::array<NRuint, N>& sizes, cl_status* err = nullptr)
		: Module(Context::getDefault(), codes, sizes, err)
	{
	}

	template<NRuint N>
	explicit Module(Context& context, const std::array<NRchar*, N>& codes, const std::array<NRuint, N>& sizes, cl_status* err = nullptr)
		: Wrapped(clCreateProgramWithSource(context, N, codes.data(), sizes.data(), err))
	{
	}

	Module(Context& context, const string& code, cl_status* err = nullptr)
		: Wrapped();
	{
		NRuint s = code.size();
		auto code_ptr = str.c_str();
		object = clCreateProgramWithSource(context, 1, &code_ptr, &s, err);
	}

	Module(Context& context, const Sources& codes, cl_status* err = nullptr)
		: Wrapped() 
	{
		std::vector<NRuint> sizes;
		std::transform(codes.cbegin(), codes.cend(), sizes.begin(), string::size);
		object = clCreateProgramWithSource(context, codes.size(), &codes[0], sizes, err);
	}

    explicit Module(const cl_program& Module, const NRbool retain = false)
        : Wrapped(Module, retain)
    {
    }

    Module(const Module& other)
        : Wrapped(other)
    {
    }

    Module(Module&& other)
        : Wrapped(other)
    {
    }

    Module& operator=(const Module& other)
    {
        return Wrapped::operator=(other);
    }

    Module& operator=(Module&& other)
    {
        return Wrapped::operator=(other);
    }

	operator cl_program() const
	{
		return object;
	}

	cl_status build(const Options& options)
	{
		return clBuildProgram(
			object, 
			1, &Device::getDefault(), 
			Module::finalizeOptions(options).c_str(), 
			nullptr, 
			nullptr);
	}

	cl_status build(const Device& device, const Options& options)
	{
		return clBuildProgram(
			object, 
			1, &device, 
			Module::finalizeOptions(options).c_str(), 
			nullptr, 
			nullptr);
	}

	cl_status build(const Devices& devices, const Options& options)
	{
		return clBuildProgram(
			object, 
			devices.size(), &devices[0], 
			Module::finalizeOptions(options).c_str(), 
			nullptr, 
			nullptr);
	}

	template<typename T>
	cl_status build(const Device& device, const Options& options, std::function<void(Module, T)> callback, T userData)
	{
		return clBuildProgram(
			object, 
			1, &device, 
			Module::finalizeOptions(options).c_str(), 
			[callback](cl_program prog, void* data){callback(Module(prog), (T) data)}, 
			userData);
	}
	
	template<typename T>
	cl_status build(const Options& options, std::function<void(Module, T)> callback, T userData)
	{
		return clBuildProgram(
			object, 
			1, &Device::getDefault(), 
			Module::finalizeOptions(options).c_str(), 
			[callback](cl_program prog, void* data){callback(Module(prog), (T) data)}, 
			userData);
	}

	template<typename T>
	cl_status build(const Devices& devices, const Options& options, void(*callback)(Module, T), T userData)
	{
		return clBuildProgram(
			object, 
			devices.size(), &devices[0], 
			Module::finalizeOptions(options).c_str(), 
			[callback](cl_program prog, void* data){callback(Module(prog), (T) data)}, 
			userData);
	}

	Kernel createKernel(const string& name, cl_status* err = nullptr)
	{
		return Kernel(clCreateKernel(object, name.c_str(), err));
	}

private:
	static string finalizeOptions(const Options& options)
	{
    	string ret = "";
    	for (auto it = options.cbegin(); it != options.cend(); ++it)
    	{
    	    ret += " " + it->getOption();
    	}

    	return ret;    
	}

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
