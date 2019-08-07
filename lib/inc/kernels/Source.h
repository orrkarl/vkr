#pragma once

#include "../general/predefs.h"

#include "../base/Context.h"
#include "../base/Device.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

namespace nr
{

namespace detail
{

class Source : private Module
{
public:
	Source(const Context& context);

	cl_status build(const Device& dev, const nr_uint renderDimension, const nr_bool debug = true);

	Kernel binRasterizer(cl_status* status = nullptr) const;

	Kernel fineRasterizer(cl_status* status = nullptr) const;

	Kernel simplexReduce(cl_status* status = nullptr) const;

	Kernel vertexReduce(cl_status* status = nullptr) const;

	Context getRenderContext() const;

	operator cl_program() const;

	nr_uint getRenderDimension() const;

private:
	nr_uint dim;
};

}

}
