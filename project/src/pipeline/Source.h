#pragma once

#include "../predefs.h"

#include "../base/Context.h"
#include "../base/Device.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "BinRasterizer.h"
#include "FineRasterizer.h"
#include "SimplexReducer.h"
#include "VertexReducer.h"

namespace nr
{

namespace detail
{

class Source : private Module
{
public:
	Source(const Context& context);

	cl_status build(const Device& dev, const nr_uint renderDimension, const nr_bool debug = true);

	BinRasterizer binRasterizer(cl_status* status = nullptr) const;

	FineRasterizer fineRasterizer(cl_status* status = nullptr) const;

	SimplexReducer simplexReducer(cl_status* status = nullptr) const;

	VertexReducer vertexReducer(cl_status* status = nullptr) const;

	operator cl_program() const;
};

}

}
