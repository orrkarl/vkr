#include "Module.h"

#include <algorithm>
#include <functional>
#include <memory>

namespace nr
{

Module::Option::Option(const string& value)
    : value(value)
{
}

const string& Module::Option::getOption() const 
{ 
    return value; 
}

Module::Macro::Macro(const string name)
    : Module::Option("-D " + name)
{
}

Module::Macro::Macro(const string name, const string value)
    : Module::Option("-D " + name + "=" + value)
{
}

Module::RenderDimension::RenderDimension(const nr_uint& dimension)
    : Module::Macro("RENDER_DIMENSION", std::to_string(dimension))
{
}

Module::CLVersion::CLVersion(const string& version)
    : Module::Option("-cl-std=CL" + version)
{
}

Module::Module()
    : Wrapped()
{
}

Module::Module(const Context& context, const string& code, cl_status* err)
    : Wrapped()
{
    size_t s = code.size();
    auto code_ptr = code.c_str();
    object = clCreateProgramWithSource(context, 1, &code_ptr, &s, err);
}

Module::Module(const Context& context, const Sources& codes, cl_status* err)
    : Wrapped() 
{
    std::vector<const nr_char*> codesRaw(codes.size());
    std::transform(codes.cbegin(), codes.cend(), codesRaw.begin(), std::mem_fn(&string::c_str));
    object = clCreateProgramWithSource(context, static_cast<nr_uint>(codes.size()), &codesRaw[0], nullptr, err);
}

Module::Module(const cl_program& module, const nr_bool retain)
    : Wrapped(module, retain)
{
}

Module::Module(const Module& other)
    : Wrapped(other)
{
}

Module::Module(Module&& other)
    : Wrapped(other)
{
}

Module& Module::operator=(const Module& other)
{
    Wrapped::operator=(other);
    return *this;
}

Module& Module::operator=(Module&& other)
{
    Wrapped::operator=(other);
    return *this;
}

Module::operator cl_program() const
{
    return get();
}

cl_program Module::get() const
{
	return object;
}

cl_status Module::build(const Device& device, const Options& options)
{
    return clBuildProgram(
        object, 
        1, &device.get(), 
        Module::finalizeOptions(options).c_str(), 
        nullptr, 
        nullptr);
}

cl_status Module::build(const Devices& devices, const Options& options)
{
    return clBuildProgram(
        object, 
		static_cast<nr_uint>(devices.size()), (const cl_device_id*) &devices[0],
        Module::finalizeOptions(options).c_str(), 
        nullptr, 
        nullptr);
}

Kernel Module::createKernel(const string& name, cl_status* err) const
{
    return createKernel(name.c_str(), err);
}

Kernel Module::createKernel(const char* name, cl_status* err) const
{
	return Kernel(clCreateKernel(object, name, err));
}

string Module::finalizeOptions(const Options& options)
{
    string ret = "";
    for (auto it = options.cbegin(); it != options.cend(); ++it)
    {
        ret += " " + it->getOption();
    }

    return ret;    
}

string Module::getBuildLog(Device device, cl_status* err) const
{
	nr_size len = 0;
	*err = clGetProgramBuildInfo(object, device, CL_PROGRAM_BUILD_LOG, len, nullptr, &len);
	std::unique_ptr<nr_char[]> str = std::make_unique<nr_char[]>(len);
	*err = clGetProgramBuildInfo(object, device, CL_PROGRAM_BUILD_LOG, len, str.get(), nullptr);
	return string(str.get());
}

Context Module::getContext(cl_status* err) const
{
	cl_context ret;
	if (err)
	{
		*err = clGetProgramInfo(object, CL_PROGRAM_CONTEXT, sizeof(ret), &ret, nullptr);
	}
	else
	{
		clGetProgramInfo(object, CL_PROGRAM_CONTEXT, sizeof(ret), &ret, nullptr);
	}

	return Context(ret, true);
}

const Module::Macro           Module::DEBUG               = Module::Macro("_DEBUG");
const Module::CLVersion       Module::CL_VERSION_22       = Module::CLVersion("2.2");
const Module::CLVersion       Module::CL_VERSION_21       = Module::CLVersion("2.1");
const Module::CLVersion       Module::CL_VERSION_20       = Module::CLVersion("2.0");
const Module::CLVersion       Module::CL_VERSION_12       = Module::CLVersion("1.2");
const Module::CLVersion       Module::CL_VERSION_11       = Module::CLVersion("1.1");
const Module::RenderDimension Module::_3D                 = Module::RenderDimension(3);
const Module::RenderDimension Module::_4D                 = Module::RenderDimension(4);
const Module::RenderDimension Module::_5D                 = Module::RenderDimension(5);

}
