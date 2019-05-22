#include <base/Buffer.h>

#include <utils/converters.h>

namespace nr
{

Buffer::Buffer(cl_mem_flags flags, const NRulong& size, void* data, cl_int* error)
{
    cl_int err;
    m_buffer = cl::Buffer(flags, size, data, &err);
    if (error != nullptr) *error = err;
}

cl_int Buffer::resize(cl_mem_flags flags, const NRuint size)
{
    cl_int err;
    m_buffer = cl::Buffer(flags, size, nullptr, &err);
    return err;
}

}