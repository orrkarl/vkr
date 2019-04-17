#pragma once

#include "../general/predefs.h"
#include "../utils/converters.h"

namespace nr
{

#define CTOR(DataType, DataTypeID)                                                              \
    Buffer(cl_mem_flags flags, const NRulong& size, DataType* data, Error* err = nullptr)       \
        : Buffer(flags, size, Type::DataTypeID, data, err)                                      \
    {                                                                                           \
    }                                                                                           \

class Buffer
{
public:
    Buffer(cl_mem_flags flags, const NRulong& size, const Type& type, Error* err = nullptr)
        : Buffer(flags, size, type, nullptr, err)
    {
    }

    Buffer(const Type& type)
        : m_type(type)
    {
    }

    CTOR(NRdouble, DOUBLE)

    CTOR(NRfloat, FLOAT)

	CTOR(NRlong, LONG)

	CTOR(NRulong, ULONG)

	CTOR(NRint, INT)

	CTOR(NRuint, UINT)

	CTOR(NRshort, SHORT)

	CTOR(NRushort, USHORT)

	CTOR(NRbyte, BYTE)

	CTOR(NRubyte, UBYTE)

	CTOR(NRchar, CHAR)

	CTOR(NRbool, BOOL)

    Type getType() const { return m_type; }

    cl::Buffer getBuffer() const { return m_buffer; }

    Error resize(cl_mem_flags flags, const NRuint size);

private:
    Buffer(cl_mem_flags flags, const NRulong& size, const Type& type, void* data = nullptr, Error* error = nullptr)
        : m_type(type)
    {
        cl_int err;
        m_buffer = cl::Buffer(flags, size, data, &err);

        if (err != CL_SUCCESS && error != nullptr) *error = utils::fromCLError(err);
    }

    Type m_type;
    cl::Buffer m_buffer;
};

#undef CTOR

}