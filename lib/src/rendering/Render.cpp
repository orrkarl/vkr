#include <iostream>

#include <general/predefs.h>

#include <rendering/Render.h>
#include <utils/converters.h>

namespace nr
{

RenderState Render::state = { { 0, 0, 0, 0 }, { 0.0f, 0.0f, 0.0f, 0.0f } };

Error Render::link()
{
    glLinkProgram(m_program);
    GLint isLinked;
    glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE)
    {
        release();
        return Error::INVALID_OPERATION;
    }

    glDetachShader(m_program, m_vertexShader->getContent());    
    glDetachShader(m_program, m_fragmentShader->getContent());
    m_isLinked = true;
    return Error::NO_ERROR;
}

Error Render::drawArrays(RenderData& obj)
{
    Error err;

    auto vertBuf = obj.getVertexBuffer();
    if (vertBuf == nullptr) return Error::INVALID_VALUE;

    glUseProgram(m_program);
    if (error::isFailure(err = utils::getLastGLError()))
    {
        return err;
    }

    err = obj.finalizeBindings();
    if (error::isFailure(err)) 
    {
        obj.unbind();
        return err;
    }

    if (error::isFailure((err = finalizeUniforms())))
    {
        return err;
    }

    glBindBuffer(GL_UNIFORM_BUFFER, m_uniBuffer);
    if (error::isFailure((err = utils::getLastGLError())))
    {
        return err;
    }

    auto itemCount = vertBuf->getSize() / vertBuf->getElementSize();
    
    glDrawArrays(utils::fromNRPrimitiveType(obj.getPrimitiveType()), 0, itemCount);
    glUseProgram(0);

    return utils::getLastGLError();
}


Error Render::finalizeUniforms()
{    
    NRuint index;
    Uniform* uniform;
    for (const auto unif : m_uniforms)
    {
        index = unif.first;
        uniform = unif.second;
        
        auto dim = uniform->getDimensions();
        auto count = uniform->getCount();
        
        switch(uniform->getDataType())
        {
            case Type::FLOAT:
                NRfloat* data;
                data = uniform->getData<NRfloat*>();
                switch (dim.first)
                {
                    case 1u:
                        switch (dim.second)
                        {
                            case 1u:
                                glUniform1f(index, data[0]);
                                return Error::NO_ERROR;            
                            case 2u:
                                glUniform2f(index, data[0], data[1]);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniform3f(index, data[0], data[1], data[2]);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniform4f(index, data[0], data[1], data[2], data[3]);
                                return Error::NO_ERROR;
                            default:
                                return Error::INVALID_VALUE;   
                        }
                    case 2u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;            
                            case 2u:
                                glUniformMatrix2fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix2x3fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix2x4fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            default:
                                return Error::INVALID_VALUE;   
                        }
                    case 3u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;            
                            case 2u:
                                glUniformMatrix3x2fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix3fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix3x4fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            default:
                                return Error::INVALID_VALUE;   
                        }
                    case 4u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;            
                            case 2u:
                                glUniformMatrix4x2fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix4x3fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix4fv(index, count, GL_FALSE, data);
                                return Error::NO_ERROR;
                            default:
                                return Error::INVALID_VALUE;   
                        }
                    default:
                        return Error::INVALID_VALUE;
                }

            default:
                return Error::INVALID_TYPE;
        }
    }

    return Error::NO_ERROR;
}


}
