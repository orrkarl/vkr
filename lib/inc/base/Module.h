#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"
#include "Kernel.h"
#include <vector>

namespace nr
{

namespace __internal
{

/**
 * Utility wrapper around cl::Module
 **/
class NR_SHARED_EXPORT Module
{

public:
	struct NR_SHARED_EXPORT Option
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

	struct NR_SHARED_EXPORT CLVersion : Option
	{
		CLVersion(const NRfloat& version)
			: Option("-cl-std=CL" + std::to_string(version))
		{
		}
	};

	struct NR_SHARED_EXPORT Macro : Option
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

	struct NR_SHARED_EXPORT RenderDimension : Macro
	{
		RenderDimension(const NRuint& dimension)
			: Macro("RENDER_DIMENSION", std::to_string(dimension))
		{
		}
	};

	static const Macro DEBUG;

	static const CLVersion CL_VERSION_20;
	static const CLVersion CL_VERSION_12;
	
	static const Option WARNINGS_ARE_ERRORS;

	static const RenderDimension _3D;
	static const RenderDimension _4D;
	static const RenderDimension _5D;

    Module() {}

    Module(const string& code, const Options& options, cl_int* err);
    Module(const std::initializer_list<string> codes, const Options& options, cl_int* err);

    template<class Params>
    Kernel<Params> makeKernel(const string& name, cl_int* err)
    {
        return Kernel<Params>(m_module, name.data(), err);
    }

    string getBuildLog(cl_int* err) const
    {
        auto ret = m_module.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault(), err);
        return ret;
    }

private:
    cl::Program m_module;
};

}

}
