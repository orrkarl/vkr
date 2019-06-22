#include <base/Module.h>

namespace nr
{

Module::Option::Option(const string& value)
    : value(value)
{
}

string Module::Option::getOption() const 
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

Module::RenderDimension::RenderDimension(const NRuint& dimension)
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

Module::Module(const string& code, cl_status* err = nullptr)
    : Module(Context::getDefault(), code, err)
{
}

Module::Module(const Sources& codes, cl_status* err = nullptr)
    : Module(Context::getDefault(), codes, err)
{
}

Module::Module(Context& context, const string& code, cl_status* err = nullptr)
    : Wrapped();
{
    NRuint s = code.size();
    auto code_ptr = str.c_str();
    object = clCreateProgramWithSource(context, 1, &code_ptr, &s, err);
}

Module::Module(Context& context, const Sources& codes, cl_status* err = nullptr)
    : Wrapped() 
{
    std::vector<NRuint> sizes;
    std::transform(codes.cbegin(), codes.cend(), sizes.begin(), string::size);
    object = clCreateProgramWithSource(context, codes.size(), &codes[0], sizes, err);
}

Module::Module(const cl_program& module, const NRbool retain = false)
    : Wrapped(Module, retain)
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
    return Wrapped::operator=(other);
}

Module& Module::operator=(Module&& other)
{
    return Wrapped::operator=(other);
}

Module::operator cl_program() const
{
    return object;
}

cl_status Module::build(const Options& options)
{
    return clBuildProgram(
        object, 
        1, &Device::getDefault(), 
        Module::finalizeOptions(options).c_str(), 
        nullptr, 
        nullptr);
}

cl_status Module::build(const Device& device, const Options& options)
{
    return clBuildProgram(
        object, 
        1, &device, 
        Module::finalizeOptions(options).c_str(), 
        nullptr, 
        nullptr);
}

cl_status Module::build(const Devices& devices, const Options& options)
{
    return clBuildProgram(
        object, 
        devices.size(), &devices[0], 
        Module::finalizeOptions(options).c_str(), 
        nullptr, 
        nullptr);
}

Kernel Module::createKernel(const string& name, cl_status* err = nullptr)
{
    return Kernel(clCreateKernel(object, name.c_str(), err));
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

const Module::Macro           Module::DEBUG               = Module::Macro("_DEBUG");
const Module::CLVersion       Module::CL_VERSION_22       = Module::CLVersion("2.2");
const Module::CLVersion       Module::CL_VERSION_21       = Module::CLVersion("2.1");
const Module::CLVersion       Module::CL_VERSION_20       = Module::CLVersion("2.0");
const Module::CLVersion       Module::CL_VERSION_12       = Module::CLVersion("1.2");
const Module::CLVersion       Module::CL_VERSION_11       = Module::CLVersion("1.1");
const Module::Option          Module::WARNINGS_ARE_ERRORS = Module::Option("-Werror");
const Module::RenderDimension Module::_3D                 = Module::RenderDimension(3);
const Module::RenderDimension Module::_4D                 = Module::RenderDimension(4);
const Module::RenderDimension Module::_5D                 = Module::RenderDimension(5);

}
