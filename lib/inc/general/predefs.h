#pragma once

#include <cstdint>
#include <string>
#include <cmath>
#include <stdio.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

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

#ifndef CL_CALLBACK
	#define CL_CALLBACK
#endif // NOT CL_CALLBACK

namespace nr
{

typedef std::basic_string<NRchar> string;

namespace error
{
	NR_SHARED NRbool isSuccess(const cl_status& err);
	NR_SHARED NRbool isFailure(const cl_status& err);
}

}

