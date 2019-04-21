#include <base/Buffer.h>

#include <utils/converters.h>

namespace nr
{

Buffer::Buffer(cl_mem_flags flags, const NRulong& size, void* data, Error* error)
{
    cl_int err;
    m_buffer = cl::Buffer(flags, size, data, &err);

    if (err != CL_SUCCESS && error != nullptr) *error = utils::fromCLError(err);
}

Error Buffer::resize(cl_mem_flags flags, const NRuint size)
{
    cl_int err;
    m_buffer = cl::Buffer(flags, size, nullptr, &err);
    return utils::fromCLError(err);
}

}