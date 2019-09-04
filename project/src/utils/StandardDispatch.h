/**
 * @file
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Implements a execution range aware, typesafe kernel
 * @version 0.6.0
 * @date 2019-08-27
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once

#include "../base/CommandQueue.h"
#include "../base/Dispatch.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

namespace nr
{

/**
 * @brief A kernel which holds it's own execution sizes
 * 
 * This class is ment to be inherited from in order to modify the execution range in a more encapsulated way
 * @tparam dim kernel execution dimension
 * @tparam Args kernel argument types
 */
template <nr_uint dim, typename... Args>
class StandardDispatch : protected TypesafeKernel<Args...>, public Dispatch<StandardDispatch<dim, Args...>>
{
public:
	StandardDispatch(const Module& module, const char* name, cl_status& err)
		: TypesafeKernel<Args...>(module, name, err), range{}
	{
	}

	StandardDispatch(const Module& module, const string& name, cl_status& err)
		: TypesafeKernel<Args...>(module, name, err), range{}
	{
	}

	StandardDispatch()
		: TypesafeKernel<Args...>(), range{}
	{
	}

	/**
	 * @brief implements Dispatch
	 * 
	 * enqueues this kernel with it's execution range
	 * @param q command queue
	 * @return cl_status internal OpenCL call status
	 */
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
