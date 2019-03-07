#pragma once

#include <vector>

#include "../general/predefs.h"
#include "../utils/converters.h"

namespace nr
{

class NR_SHARED_EXPORT Shader
{
public:

    Shader(
        const std::string& code,
        const NRbool shouldCompile,
        const Role shaderType,
        Error& err)
        : m_type(shaderType),
          m_shader(glCreateShader(utils::fromNRShaderType(shaderType))),
          m_isCompiled(false)
    {
        auto str = code.c_str();
        GLint len = code.size();
        GLint success;
        glShaderSource(m_shader, 1, &str, &len);

        if (shouldCompile)
        {
            err = compile();
        }
    }

    ~Shader() { release(); }

    Error compile() 
    {
        if (m_isCompiled) return Error::INVALID_OPERATION;

        Error err = Error::NO_ERROR;
        glCompileShader(m_shader);
        if (error::isSuccess(err = utils::getLastGLError()))
        {
            GLint success;
            glGetShaderiv(m_shader, GL_COMPILE_STATUS, &success);
            if (success == GL_FALSE)
            {
                err = Error::INVALID_OPERATION;
            }
        }

        m_isCompiled = error::isSuccess(err);
        return err;
    }

    void release()
    {
        glDeleteShader(m_shader);
        m_shader = 0u;
    }

    NRbool isCompiled() const { return m_isCompiled; }

    GLuint getContent() const { return m_shader; }

    Role getShaderType() const { return m_type; }

    string getCompilerLog(Error& err)
    {
        GLint logLength;

        glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &logLength);
        if (error::isFailure(err = utils::getLastGLError()))
        {
            return string{};
        }

        if (logLength > 0)
        {
            std::vector<NRchar> log(logLength);
            glGetShaderInfoLog(m_shader, log.size(), &logLength, log.data());
            return string(log.begin(), log.end());
        }

        return string{};
    }

private:
    GLuint m_shader;
    const Role m_type;
    NRbool m_isCompiled;

};

}