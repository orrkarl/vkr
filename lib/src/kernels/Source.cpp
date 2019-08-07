#include <kernels/Source.h>

namespace nr 
{ 

namespace detail 
{

Source::Source(const Context& context)
	: Module(context, Module::Sources{ clcode::base, clcode::bin_rasterizer, clcode::fine_rasterizer, clcode::simplex_reducing, clcode::vertex_shading }), dim(0)
{
}

cl_status Source::build(const Device& dev, const nr_uint renderDimension, const nr_bool debug)
{
	Module::Options opts;
	opts.push_back(Module::CL_VERSION_12);
	opts.push_back(Module::RenderDimension(renderDimension));
	if (debug) opts.push_back(Module::DEBUG);

	dim = renderDimension;

	return Module::build(dev, opts);
}

BinRasterizerKernel Source::binRasterizer(cl_status* status) const
{
	return BinRasterizerKernel(*this, status);
}

FineRasterizerKernel Source::fineRasterizer(cl_status* status) const
{
	return FineRasterizerKernel(*this, status);
}

SimplexReduceKernel Source::simplexReduce(cl_status* status) const
{
	return SimplexReduceKernel(*this, status);
}

VertexReduceKernel Source::vertexReduce(cl_status* status) const
{
	return VertexReduceKernel(*this, status);
}

Context Source::getRenderContext() const
{
	return Module::getContext();
}

Source::operator cl_program() const
{
	return Module::get();
}

nr_uint Source::getRenderDimension() const
{
	return dim;
}

} 

}
