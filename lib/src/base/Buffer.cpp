#include <base/Buffer.h>

namespace nr
{

Error Buffer::resize(cl_mem_flags flags, const NRuint size)
{
    cl_int err;
    m_buffer = cl::Buffer(flags, size, nullptr, &err);
    return utils::fromCLError(err);
}

}