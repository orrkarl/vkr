#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* VERTEX_REDUCE_KERNEL_NAME;
class VertexReduceKernel : public TypesafeKernel<Buffer, Buffer, Buffer, Buffer>
{
public:
	VertexReduceKernel(const Module& module, cl_status* err = nullptr);
};

namespace clcode
{

NRAPI extern const char* vertex_shading;

}

}

}