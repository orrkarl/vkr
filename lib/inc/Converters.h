#include "predefs.h"

#pragma once

namespace nr
{

GLenum fromNRType(Type type)
{
    switch(type)
    {
        case Type::BYTE:    return GL_BYTE;
        case Type::UBYTE:   return GL_UNSIGNED_BYTE;
        
        case Type::SHORT:   return GL_SHORT;
        case Type::USHORT:  return GL_UNSIGNED_SHORT;
        
        case Type::BOOL:    return GL_BOOL;

        case Type::INT:     return GL_INT;
        case Type::UINT:    return GL_UNSIGNED_INT;

        case Type::FLOAT:   return GL_FLOAT;
        case Type::DOUBLE:  return GL_DOUBLE;
        
        default:
            return 0; // type is not supported in core GL 
    }
}

Error fromGLError(GLint err)
{
    switch(err)
    {
        case GL_INVALID_ENUM:   return Error::INVALID_TYPE;
        case GL_INVALID_VALUE:  return Error::INVALID_VALUE;
        case GL_NO_ERROR:       return Error::NO_ERROR;

        default:
            return Error::UNKNOWN_ERROR;
    }
}

Error getLastGLError()
{
    return fromGLError(glGetError());
}

GLenum fromNRShaderType(const ShaderType& type)
{
    switch(type)
    {
        case ShaderType::VERTEX:    return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:  return GL_FRAGMENT_SHADER;

        default:
            return 0;
    }
}

}