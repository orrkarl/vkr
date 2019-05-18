#pragma once

#include <cstdint>
#include <string>
#include <cmath>
#include <stdio.h>

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>

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

#ifdef _MSC_VER
	#define NR_SHARED_EXPORT __declspec(dllexport)
#else
	#define NR_SHARED_EXPORT
#endif // _MSC_VER

namespace nr
{

typedef std::basic_string<NRchar> string;

enum class NR_SHARED_EXPORT Type : NRint
{
	DOUBLE, FLOAT, ULONG, LONG, UINT, INT, USHORT, SHORT, UBYTE, BYTE, CHAR, BOOL, RAW
};

namespace type
{
	NR_SHARED_EXPORT NRuint getByteSize(const Type& type);
}

enum class NR_SHARED_EXPORT Error : NRint
{
	NO_ERROR=0,
	
	INVALID_TYPE		= 1,
	INVALID_VALUE		= 2,
	INVALID_OPERATION	= 3,
	
	HOST_OUT_OF_MEMORY		= 4,
	DEVIDE_OUT_OF_MEMORY	= 5,

	INVALID_COMPILER_OPTIONS	= 6,
	COMPILATION_FAILURE			= 7,
	INVALID_COMPILER			= 8,

	INVALID_ARGS	= 9,
	
	UNKNOWN_ERROR	= 10
};


std::ostream& operator<<(std::ostream& os, const Error& error);

namespace error
{
	NR_SHARED_EXPORT NRbool isSuccess(const Error& err);
	NR_SHARED_EXPORT NRbool isFailure(const Error& err);
}

}

