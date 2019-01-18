#include <pipeline/Rasterizer.h>
#include <pipeline/PipelineCL.h>

#include <utils/files.h>

namespace nr
{

namespace __internal
{

Rasterizer::Rasterizer(const NRuint& dimension, cl_int& err)
    : Stage(rasterizer_generic_dim, err),
      h_info(dimension)
{
    d_info = cl::Buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Info), &h_info, &err);

    points      = cl::Kernel(code, "points", &err);
    lines       = cl::Kernel(code, "lines", &err);
    simplices   = cl::Kernel(code, "simplices", &err);
}

Error Rasterizer::rasterize(
    const cl::Buffer& src,
    const cl::Buffer& dest, 
    const cl::CommandQueue& queue, 
    const NRuint elementCount, 
    const Primitive primitive)
{
    Error err;
    if (error::isFailure(err = updateInfo(queue))) return err;
    
    auto global_range = cl::NDRange(elementCount);

    switch (primitive)
    {
        case Primitive::POINTS:
            points.setArg(0, d_info);
            points.setArg(1, src);
            points.setArg(2, dest);
            return utils::fromCLError(
                queue.enqueueNDRangeKernel(points, cl::NullRange, global_range));
        case Primitive::LINES:
            lines.setArg(0, d_info);
            lines.setArg(1, src);
            lines.setArg(2, dest);
            return utils::fromCLError(
                queue.enqueueNDRangeKernel(lines, cl::NullRange, global_range));
        case Primitive::K_SIMPLICES:
            simplices.setArg(0, d_info);
            simplices.setArg(1, src);
            simplices.setArg(2, dest);
            return utils::fromCLError(
                queue.enqueueNDRangeKernel(simplices, cl::NullRange, global_range));
        default:
            return Error::INVALID_VALUE;
    }
}

Error Rasterizer::updateInfo(const cl::CommandQueue& queue)
{
    auto err = queue.enqueueWriteBuffer(d_info, CL_TRUE, 0, sizeof(h_info), &h_info);
    infoUpdated = err == CL_SUCCESS;
    return utils::fromCLError(err);
}

}

}
