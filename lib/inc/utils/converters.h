#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace utils
{

GLenum fromNRType(Type type);

Error fromGLError(GLenum err);

Error getLastGLError();

GLenum fromNRShaderType(const Role& type);

GLenum fromNRPrimitiveType(const Primitive& type);

Error fromCLError(const cl_int& err);

}

}