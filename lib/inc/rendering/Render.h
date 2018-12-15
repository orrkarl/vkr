#pragma once

#include "../general/predefs.h"

#include "../shaders/Shader.h"

#include "RenderData.h"
#include "RenderState.h"

namespace nr
{

class Render
{
private:
    Shader* m_vertexShader;
    Shader* m_fragmentShader;

    static RenderState state;

    GLuint m_program = 0;

public:
    Render()
        : m_program(glCreateProgram())
    {
    }

    ~Render() { release(); }

    void release()
    {
        glDeleteProgram(m_program);
        m_program = 0;
    }

    Error drawArrays(RenderData& drawable) const;

    Error link();

    Error bindShader(Shader* shader)
    {
        switch(shader->getShaderType())
        {
            case ShaderType::VERTEX:
                m_vertexShader = shader;
                glAttachShader(m_program, shader->getContent());
                return utils::getLastGLError();
            
            case ShaderType::FRAGMENT:
                m_fragmentShader = shader;
                glAttachShader(m_program, shader->getContent());
                return utils::getLastGLError();

            default:
                return Error::INVALID_TYPE;
        }
    }

    static void viewPort(NRint x, NRint y, NRuint w, NRuint h)
    {
        state.wnd.set(x, y, w, h);
        glViewport(x, y, w, h);
    }

    static void setClearColor(const NRfloat r, const NRfloat g, const NRfloat b, const NRfloat a)
    {
        state.color.set(r, g, b, a);
        glClearColor(r, g, b, a);
    }

    static void clearColor()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

}