#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* SIMPLEX_REDUCE_KERNEL_NAME;

class SimplexReduceKernel : public TypesafeKernel<Buffer, Buffer>
{
public:
	SimplexReduceKernel(const Module& module, cl_status* err = nullptr);
};

namespace clcode
{

// Reducing simplexes to their triangles
NRAPI extern const char* simplex_reducing;

}

}

}
