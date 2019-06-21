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

typedef cl_int cl_status;

#ifndef NR_SHARED

#ifdef _MSC_VER
	#define NR_SHARED __declspec(dllimport)
#else
	#define NR_SHARED
#endif // _MSC_VER

#endif // NOT NR_SHARED

namespace nr
{

typedef std::basic_string<NRchar> string;

enum class NR_SHARED Type : NRint
{
	DOUBLE, FLOAT, ULONG, LONG, UINT, INT, USHORT, SHORT, UBYTE, BYTE, CHAR, BOOL, RAW
};

namespace type
{
	NR_SHARED NRuint getByteSize(const Type& type);
}

namespace error
{
	NR_SHARED NRbool isSuccess(const cl_int& err);
	NR_SHARED NRbool isFailure(const cl_int& err);
}

}

