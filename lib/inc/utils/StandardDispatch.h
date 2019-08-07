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
		: TypesafeKernel(module, name, err), range{}
	{
	}

	StandardDispatch(const Module& module, const string& name, cl_status* err)
		: TypesafeKernel(module, name, err), range{}
	{
	}

	StandardDispatch()
		: TypesafeKernel(), range{}
	{
	}

	cl_status consume(const CommandQueue& q) const
	{
		return q.enqueueKernelCommand(TypesafeKernel::get(), range);
	}

	NDExecutionRange<dim> getExecutionRange() const
	{
		return range;
	}

protected:
	NDExecutionRange<dim> range;
};

}
