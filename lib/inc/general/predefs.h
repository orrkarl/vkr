#include <cstdint>
#include <string>

#include <GL/glew.h>

#pragma once

typedef double NRdouble;
typedef float NRfloat;

typedef int64_t NRlong;
typedef uint64_t NRulong;

typedef int32_t NRint;
typedef uint32_t NRuint;

typedef int16_t NRshort;
typedef uint16_t NRushort;

typedef int8_t NRbyte;
typedef uint8_t NRubyte;

typedef char NRchar;
typedef bool NRbool;

namespace nr
{

typedef std::basic_string<NRchar> string;

enum class Type : int
{
	DOUBLE, FLOAT, ULONG, LONG, UINT, INT, USHORT, SHORT, UBYTE, BYTE, CHAR, BOOL
};

namespace type
{
	NRuint getByteSize(const Type& type)
	{
		switch (type)
		{
			case Type::DOUBLE:
				return sizeof(NRdouble);

			case Type::FLOAT: 
				return sizeof(NRfloat);

			case Type::ULONG:
				return sizeof(NRulong);

			case Type::LONG:
				return sizeof(NRlong);

			case Type::UINT: 
				return sizeof(NRuint);

			case Type::INT:
				return sizeof(NRint);

			case Type::USHORT: 
				return sizeof(NRushort);
				
			case Type::SHORT:
				return sizeof(NRshort);

			case Type::CHAR:
				return sizeof(NRchar);

			case Type::BOOL:
				return sizeof(NRbool); 
			
			default:
				return 0u;
		}
	}
}

enum class Error : int
{
	NO_ERROR=0, INVALID_TYPE, INVALID_VALUE, INVALID_OPERATION, UNKNOWN_ERROR
};

namespace error
{
	NRbool isSuccess(const Error& err) { return err == Error::NO_ERROR; }
	NRbool isFailure(const Error& err) { return err != Error::NO_ERROR; }
}

enum class ShaderType : int
{
	VERTEX, FRAGMENT
};

enum class Primitive : int
{
	POINTS, LINES, TRIANGLES
};

}

