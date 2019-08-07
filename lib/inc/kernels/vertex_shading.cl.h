#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Dispatch.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* VERTEX_REDUCE_KERNEL_NAME;
class VertexReduceKernel : private TypesafeKernel<Buffer, Buffer, Buffer, Buffer>, Dispatch<VertexReduceKernel> 
{
public:
	VertexReduceKernel(const Module& module, cl_status* err = nullptr);

	VertexReduceKernel();

	cl_status setSimplexInputBuffer(const Buffer& in);

	cl_status setNearPlaneBuffer(const Buffer& near);

	cl_status setFarPlaneBuffer(const Buffer& far);

	cl_status setSimplexOutputBuffer(const Buffer& out);

	cl_status consume(const CommandQueue& q);

	void setExecutionRange(const nr_uint primitiveCount);

	NDExecutionRange<1> getExecutionRange() const;

	using TypesafeKernel<Buffer, Buffer, Buffer, Buffer>::operator cl_kernel;

private:
	NDExecutionRange<1> m_range;

	static constexpr const nr_uint INPUT_BUFFER = 0;
	static constexpr const nr_uint NEAR_PLANE_BUFFER = 1;
	static constexpr const nr_uint FAR_PLANE_BUFFER = 2;
	static constexpr const nr_uint OUTPUT_BUFFER = 3;
};

namespace clcode
{

NRAPI extern const char* vertex_shading;

}

}

}