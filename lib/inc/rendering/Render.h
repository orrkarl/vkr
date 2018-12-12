#include "../predefs.h"

#include "shaders/Shader.h"
#include "DrawObject.h"

#pragma once

namespace nr
{

class Render
{
private:
    Shader* m_vertexShader;
    Shader* m_fragmentShader;

    GLuint m_program;

public:
    Error drawArrays(const DrawObject& drawable) const;

    Error link();

    Error bindShader(Shader* shader)
    {
        switch(shader->getShaderType())
        {
            case ShaderType::VERTEX:
                m_vertexShader = shader;
                return Error::NO_ERROR;
            
            case ShaderType::FRAGMENT:
                m_fragmentShader = shader;
                return Error::NO_ERROR;

            default:
                return Error::INVALID_TYPE;
        }
    }

};

}