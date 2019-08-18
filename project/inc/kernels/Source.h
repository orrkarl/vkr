#pragma once

#include "../general/predefs.h"

#include "../base/Context.h"
#include "../base/Device.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "../kernels/base.cl.h"
#include "../kernels/bin_rasterizer.cl.h"
#include "../kernels/fine_rasterizer.cl.h"
#include "../kernels/simplex_reducing.cl.h"
#include "../kernels/vertex_shading.cl.h"

namespace nr
{

namespace detail
{

class Source : private Module
{
public:
	Source(const Context& context);

	cl_status build(const Device& dev, const nr_uint renderDimension, const nr_bool debug = true);

	BinRasterizerKernel binRasterizer(cl_status* status = nullptr) const;

	FineRasterizerKernel fineRasterizer(cl_status* status = nullptr) const;

	SimplexReduceKernel simplexReduce(cl_status* status = nullptr) const;

	VertexReduceKernel vertexReduce(cl_status* status = nullptr) const;

	operator cl_program() const;
};

}

}
