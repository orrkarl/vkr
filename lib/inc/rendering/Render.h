#pragma once

#include "../general/predefs.h"

#include "../shaders/Shader.h"

#include "RenderData.h"
#include "RenderState.h"

namespace nr
{

class Render
{
public:

    Render()
        : m_program(glCreateProgram()),
          m_isLinked(false)
    {
        glGenBuffers(1, &m_uniBuffer);
    }

    Render(Error& linkError)
        : Render()
    {
        if (error::isFailure(linkError = link()))
        {
            release();
        }
    }

    ~Render() { release(); }

    void release()
    {
        glDeleteProgram(m_program);
        m_program = 0;
        glDeleteBuffers(1, &m_uniBuffer);
        m_uniBuffer = 0;
    }

    Error drawArrays(RenderData& drawable);

    Error link();

    Error bindShader(Shader* shader)
    {
        switch(shader->getShaderType())
        {
            case Role::VERTEX:
                m_vertexShader = shader;
                glAttachShader(m_program, shader->getContent());
                return utils::getLastGLError();
            
            case Role::FRAGMENT:
                m_fragmentShader = shader;
                glAttachShader(m_program, shader->getContent());
                return utils::getLastGLError();

            default:
                return Error::INVALID_TYPE;
        }
    }

    string getLinkInfo()
    {
        GLint logLength;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);
        std::vector<NRchar> log;
        glGetProgramInfoLog(m_program, log.size(), &logLength, log.data());
        return string(log.begin(), log.end());
    }

	Error bindUniform(const NRuint index, Uniform* buffer)
	{
		if (m_uniforms.count(index) != 0)
		{
			if (buffer == nullptr)
			{
				m_uniforms.erase(index);
				return Error::NO_ERROR;
			}
			else
			{
				m_uniforms.insert({ index, buffer });
				return Error::NO_ERROR;
			}
		}
		if (isIndexBound(index))
		{
			return Error::INVALID_OPERATION;
		}

		m_uniforms.insert({ index, buffer });
		return Error::NO_ERROR;
	}

	Error bindUniformBuffer(Buffer* buffer)
	{
        if (buffer->getDataType() != Type::RAW) return Error::INVALID_OPERATION;
		m_uniformBuffer = buffer;
        return Error::NO_ERROR;
	}
	
    Error finalizeUniforms();

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

private:
    Shader* m_vertexShader;
    Shader* m_fragmentShader;
    NRbool m_isLinked;
    GLuint m_program = 0;

    std::map<NRuint, Uniform*> m_uniforms{};
    Buffer* m_uniformBuffer;
    GLuint m_uniBuffer = 0u;

    static RenderState state;

	NRbool isIndexBound(const NRuint index) const
	{
		return index == 0u || m_uniforms.count(index) != 0;
	}
};

}