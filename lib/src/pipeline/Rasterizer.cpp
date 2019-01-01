#include <pipeline/Rasterizer.h>

#include <utils/files.h>


namespace nr
{

Rasterizer::Rasterizer(const NRuint& dimension, Error& err)
    : Stage(
        utils::loadResourceFile("kernels/rasterizer_dim_generic.cl", err), 
        "rasterize", 
        dimension,
        err),
      h_info(0, 0, dimension)
{
    cl_int error;
    d_info = cl::Buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Info), &h_info, &error);

    if (error != CL_SUCCESS)
    {
        err = utils::fromCLError(error);
    }

}

Error Rasterizer::apply(const cl::Buffer& src, const cl::Buffer& dest, const cl::CommandQueue& queue)
{
    Error err;
    if (error::isFailure(err = update(queue))) return err;

    auto kernel = getKernel();
    
    if (error::isFailure(err = utils::fromCLError(kernel->setArg(0, d_info)))) return err;
    if (error::isFailure(err = utils::fromCLError(kernel->setArg(1, src))))    return err;
    if (error::isFailure(err = utils::fromCLError(kernel->setArg(2, dest))))   return err;
    // TODO: find a way to make the work group sizes right
    return utils::fromCLError(queue.enqueueNDRangeKernel(*kernel, cl::NullRange, cl::NDRange(100)));
}

Error Rasterizer::updateInfo(const cl::CommandQueue& queue)
{
    auto err = queue.enqueueWriteBuffer(d_info, CL_TRUE, 0, sizeof(h_info), &h_info);
    infoUpdated = err == CL_SUCCESS;
    return utils::fromCLError(err);
}

}
