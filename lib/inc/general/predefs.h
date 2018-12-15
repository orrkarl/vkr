#pragma once

#include <cstdint>
#include <string>

#include <GL/glew.h>

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
	NRuint getByteSize(const Type& type);
}

enum class Error : int
{
	NO_ERROR=0, INVALID_TYPE, INVALID_VALUE, INVALID_OPERATION, UNKNOWN_ERROR
};

namespace error
{
	NRbool isSuccess(const Error& err);
	NRbool isFailure(const Error& err);
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

