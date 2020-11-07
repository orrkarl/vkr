/**
 * @file predefs.h
 * @author Orr Karl (karlor041@gmail.com)
 * @brief Typedefs and macros used throwout the code
 * @version 0.7.0
 * @date 2020-11-07
 *
 */

#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

using F32 = cl_float;
using F64 = cl_double;

using I8 = cl_int8;
using U8 = cl_uint8;
using I16 = cl_int16;
using U16 = cl_uint16;
using I32 = cl_int;
using U32 = cl_uint;
using I64 = cl_long;
using U64 = cl_long;

using Bool = cl_bool;
