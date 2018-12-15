#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace utils
{

GLenum fromNRType(Type type);

Error fromGLError(GLint err);

Error getLastGLError();

GLenum fromNRShaderType(const ShaderType& type);

GLenum fromNRPrimitiveType(const Primitive& type);

}

}