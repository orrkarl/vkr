#pragma once


#include "../general/predefs.h"

#include "Shader.h"


#define ShaderSubType(name, shaderType)                                     \
    class name : Shader                                                     \
    {                                                                       \
    public:                                                                 \
        name(const string& code, const NRbool compile, Error& err)          \
            : Shader(code, compile, ShaderType::shaderType, err)            \
        {                                                                   \
        }                                                                   \
    };

namespace nr
{

ShaderSubType(VertexShader, VERTEX)

ShaderSubType(FragmentShader, FRAGMENT)

}