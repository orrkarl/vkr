#include "Source.h"

#include "../kernels/base.cl.h"
#include "../kernels/bin_rasterizer.cl.h"
#include "../kernels/fine_rasterizer.cl.h"
#include "../kernels/vertex_reducer.cl.h"

namespace nr 
{ 

namespace detail 
{

Source::Source(const Context& context, cl_status& status)
	: Module(context, Module::Sources{ clcode::base, clcode::bin_rasterizer, clcode::fine_rasterizer, clcode::vertex_reduce }, status)
{
}

cl_status Source::build(const Device& dev, const nr_uint renderDimension, const nr_uint binBatchSize, const nr_bool debug)
{
	Module::Options opts;
	opts.push_back(Module::CL_VERSION_12);
	opts.push_back(Module::Macro("BATCH_COUNT", std::to_string(binBatchSize)));
	if (debug) opts.push_back(Module::DEBUG);

	return Module::build(dev, opts);
}

BinRasterizer Source::binRasterizer(cl_status& status) const
{
	return BinRasterizer(*this, status);
}

FineRasterizer Source::fineRasterizer(cl_status& status) const
{
	return FineRasterizer(*this, status);
}

VertexReducer Source::vertexReducer(cl_status& status) const
{
	return VertexReducer(*this, status);
}

Source::operator cl_program() const
{
	return Module::get();
}

} 

}
