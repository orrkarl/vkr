#pragma once

#include "../base/CommandQueue.h"
#include "../base/Dispatch.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

namespace nr
{

template <nr_uint dim, typename... Args>
class StandardDispatch : protected TypesafeKernel<Args...>, public Dispatch<StandardDispatch<dim, Args...>>
{
public:
	StandardDispatch(const Module& module, const char* name, cl_status* err)
		: TypesafeKernel<Args...>(module, name, err), range{}
	{
	}

	StandardDispatch(const Module& module, const string& name, cl_status* err)
		: TypesafeKernel<Args...>(module, name, err), range{}
	{
	}

	StandardDispatch()
		: TypesafeKernel<Args...>(), range{}
	{
	}

	cl_status consume(const CommandQueue& q) const
	{
#ifdef _DEBUG
		if (!std::all_of(setIndices.cbegin(), setIndices.cend(), [](const nr_bool b) {return b; }))
		{
			std::cerr << "Not all kernel arguments are set - { ";
			for (auto i = 0u; i < sizeof...(Args); ++i)
			{
				if (!setIndices[i])
				{
					std::cerr << i << " ";
				}
			}
			std::cerr << "}\n";
		}
#endif
		return q.enqueueKernelCommand(TypesafeKernel<Args...>::get(), range);
	}

	NDExecutionRange<dim> getExecutionRange() const
	{
		return range;
	}

	using TypesafeKernel<Args...>::operator cl_kernel;

protected:
	NDExecutionRange<dim> range;
};

}
