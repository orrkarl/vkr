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

using f32 = cl_float;
using f64 = cl_double;

using i8 = cl_int8;
using u8 = cl_uint8;
using i16 = cl_int16;
using u16 = cl_uint16;
using i32 = cl_int;
using u32 = cl_uint;
using i64 = cl_long;
using u64 = cl_long;
