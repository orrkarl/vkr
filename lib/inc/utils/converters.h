#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace utils
{

NR_SHARED_EXPORT GLenum fromNRType(Type type);

NR_SHARED_EXPORT Error fromGLError(GLenum err);

NR_SHARED_EXPORT Error getLastGLError();

NR_SHARED_EXPORT GLenum fromNRShaderType(const Role& type);

NR_SHARED_EXPORT GLenum fromNRPrimitiveType(const Primitive& type);

NR_SHARED_EXPORT Error fromCLError(const cl_int& err);

}

}