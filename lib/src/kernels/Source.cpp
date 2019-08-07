#include <kernels/Source.h>

#include <kernels/base.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <kernels/simplex_reducing.cl.h>
#include <kernels/vertex_shading.cl.h>

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

Kernel Source::binRasterizer(cl_status* status) const
{
	return Module::createKernel(clcode::BIN_RASTER_KERNEL_NAME, status);
}

Kernel Source::fineRasterizer(cl_status* status) const
{
	return Module::createKernel(clcode::FINE_RASTER_KERNEL_NAME, status);
}

Kernel Source::simplexReduce(cl_status* status) const
{
	return Module::createKernel(clcode::SIMPLEX_REDUCE_KERNEL_NAME, status);
}

Kernel Source::vertexReduce(cl_status* status) const
{
	return Module::createKernel(clcode::VERTEX_SHADE_KERNEL_NAME, status);
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
