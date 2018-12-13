#include "../general/predefs.h"

#include "../shaders/Shader.h"

#include "DrawObject.h"
#include "RenderState.h"

#pragma once

namespace nr
{

class Render
{
private:
    Shader* m_vertexShader;
    Shader* m_fragmentShader;

    RenderState m_state;

    GLuint m_program;

    Error bind(const DrawObject& obj, GLuint* vaoptr) const;

    void unbind(GLuint* vaoptr) const;

public:
    Render()
        : m_program(glCreateProgram())
    {
    }

    ~Render() { release(); }

    void release()
    {
        glDeleteProgram(m_program);
    }

    Error drawArrays(const DrawObject& drawable) const;

    Error link();

    Error bindShader(Shader* shader)
    {
        switch(shader->getShaderType())
        {
            case ShaderType::VERTEX:
                m_vertexShader = shader;
                glAttachShader(m_program, shader->getContent());
                return getLastGLError();
            
            case ShaderType::FRAGMENT:
                m_fragmentShader = shader;
                glAttachShader(m_program, shader->getContent());
                return getLastGLError();

            default:
                return Error::INVALID_TYPE;
        }
    }

};

}