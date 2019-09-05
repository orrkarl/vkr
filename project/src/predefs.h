/**
 * @file predefs.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Typedefs and macros used throwout the code
 * @version 0.5.9
 * @date 2019-06-30
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#pragma once

#include <stdexcept>
#include <string>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

using nr_double = double;
using nr_float = float;

using nr_long  = int64_t;
using nr_ulong = uint64_t;

using nr_int = int32_t;
using nr_uint = uint32_t;

using nr_short = int16_t;
using nr_ushort = uint16_t;

using nr_byte = int8_t;
using nr_ubyte = uint8_t;

using nr_char = char;
using nr_bool = bool;

using nr_size = size_t;

#ifdef NR_EXPORT

#ifdef _MSC_VER
	#define NRAPI __declspec(dllexport)
#else
	#define NRAPI
#endif // _MSC_VER

#else

#ifdef _MSC_VER
	#define NRAPI __declspec(dllimport)
#else
	#define NRAPI

#endif // _MSC_VER

#endif // NR_EXPORT	

namespace nr
{

using string = std::basic_string<nr_char>;

class Error : public std::exception
{
public:
	Error(const char* desc)
		: std::exception(desc)
	{
	}
};

}

