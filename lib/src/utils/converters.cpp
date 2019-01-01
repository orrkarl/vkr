#include <utils/converters.h>
#include <shaders/Shader.h>
#include <rendering/RenderData.h>


namespace nr
{

namespace utils
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
        
        case Type::RAW:     
            return 0; // this is not the default because i will probably change it in the future

        default:
            return 0; // type is not supported in core GL 
    }
}

Error fromGLError(GLenum err)
{
    switch(err)
    {
        case GL_INVALID_ENUM:       return Error::INVALID_TYPE;
        case GL_INVALID_VALUE:      return Error::INVALID_VALUE;
        case GL_INVALID_OPERATION:  return Error::INVALID_OPERATION;
        case GL_NO_ERROR:           return Error::NO_ERROR;

        default:
            return Error::UNKNOWN_ERROR;
    }
}

Error getLastGLError()
{
    return fromGLError(glGetError());
}

GLenum fromNRShaderType(const Role& type)
{
    switch(type)
    {
        case Role::VERTEX:    return GL_VERTEX_SHADER;
        case Role::FRAGMENT:  return GL_FRAGMENT_SHADER;

        default:
            return 0;
    }
}

GLenum fromNRPrimitiveType(const Primitive& type)
{
    switch(type)
    {
        case Primitive::POINTS:     return GL_POINTS;
        case Primitive::LINES:      return GL_LINES;
        case Primitive::TRIANGLES:  return GL_TRIANGLES;

        default:
            return 0;
    }
}

Error fromCLError(const cl_int& err)
{
    switch(err)
    {
        case CL_SUCCESS:
            return Error::NO_ERROR;

        case CL_INVALID_PROGRAM:
        case CL_INVALID_PROGRAM_EXECUTABLE:
        case CL_INVALID_KERNEL_NAME:
        case CL_INVALID_VALUE:
            return Error::INVALID_VALUE;

        case CL_INVALID_KERNEL_DEFINITION:
            return Error::INVALID_TYPE;

        case CL_OUT_OF_RESOURCES:
        case CL_OUT_OF_HOST_MEMORY:
            return Error::INVALID_OPERATION;
        
        default:
            return Error::UNKNOWN_ERROR;
    }
}

}

}
