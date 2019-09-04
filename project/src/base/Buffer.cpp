#include "Buffer.h"

namespace nr
{

Buffer::Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, cl_status& err)
	: Buffer(context, flags, size, nullptr, err)
{
}

Buffer::Buffer(const Context& context, const cl_mem_flags& flags, const nr_size& size, void* data, cl_status& error)
	: Wrapped(clCreateBuffer(context, flags, size, data, &error))
{
}

Buffer::Buffer()
	: Wrapped()
{
}

Buffer::Buffer(const cl_mem& buffer, const nr_bool retain)
	: Wrapped(buffer, retain)
{
}

Buffer::Buffer(const Buffer& other)
	: Wrapped(other)
{
}

Buffer::Buffer(Buffer&& other)
	: Wrapped(other)
{
}

Buffer& Buffer::operator=(const Buffer& other)
{
	Wrapped::operator=(other);
	return *this;
}

Buffer& Buffer::operator=(Buffer&& other)
{
	Wrapped::operator=(other);
	return *this;
}

Buffer::operator cl_mem() const
{
	return object;
}

const cl_mem& Buffer::get() const
{
	return object;
}

nr_size Buffer::size(cl_status& err) const
{
	nr_size ret = 0;
	err = clGetMemObjectInfo(object, CL_MEM_SIZE, sizeof(nr_size), &ret, nullptr);
	return ret;
}


}