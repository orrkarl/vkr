#include <iostream>

#include <general/predefs.h>

#include <rendering/Render.h>
#include <utils/converters.h>

namespace nr
{

RenderState Render::state = {};

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
    return utils::getLastGLError();
}

Error Render::drawArrays(RenderData& obj)
{
    Error err;

    auto vertBuf = obj.getVertexBuffer();
    if (vertBuf == nullptr) return Error::INVALID_VALUE;

    glUseProgram(m_program);
    if (error::isFailure(err = utils::getLastGLError()))
    {
        printf("in use program\n");
        return err;
    }

    if (error::isFailure(err = obj.finalizeBindings())) 
    {
        obj.unbind();
        printf("in fainalize bindings\n");
        return err;
    }

    if (!isUpToDate())
    {
        if (error::isFailure((err = finalizeUniforms())))
        {
            printf("in finalize uniforms\n");
            return err;
        }
        update();
    }
    
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniBuffer);
    if (error::isFailure((err = utils::getLastGLError())))
    {
        printf("in binding uniforms buffer\n");
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
                                break;           
                            case 2u:
                                glUniform2f(index, data[0], data[1]);
                                break;           
                            case 3u:
                                glUniform3f(index, data[0], data[1], data[2]);
                                break;           
                            case 4u:
                                glUniform4f(index, data[0], data[1], data[2], data[3]);
                                break;           
                            default:
                                return Error::INVALID_VALUE;   
                        }
                        break;
                    case 2u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;
                            case 2u:
                                glUniformMatrix2fv(index, count, GL_FALSE, data);
                                break;           
                            case 3u:
                                glUniformMatrix2x3fv(index, count, GL_FALSE, data);
                                break;           
                            case 4u:
                                glUniformMatrix2x4fv(index, count, GL_FALSE, data);
                                break;           
                            default:
                                return Error::INVALID_VALUE;   
                        }
                        break;
                    case 3u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;            
                            case 2u:
                                glUniformMatrix3x2fv(index, count, GL_FALSE, data);
                                break;           
                            case 3u:
                                glUniformMatrix3fv(index, count, GL_FALSE, data);
                                break;           
                            case 4u:
                                glUniformMatrix3x4fv(index, count, GL_FALSE, data);
                                break;           
                            default:
                                return Error::INVALID_VALUE;   
                        }
                        break;
                    case 4u:
                        switch (dim.second)
                        {
                            case 1u:
                                return Error::INVALID_VALUE;            
                            case 2u:
                                glUniformMatrix4x2fv(index, count, GL_FALSE, data);
                                break;           
                            case 3u:
                                glUniformMatrix4x3fv(index, count, GL_FALSE, data);
                                break;           
                            case 4u:
                                glUniformMatrix4fv(index, count, GL_FALSE, data);
                                break;           
                            default:
                                return Error::INVALID_VALUE;   
                        }
                        break;
                    default:
                        return Error::INVALID_VALUE;
                }
                break;
            default:
                return Error::INVALID_TYPE;
        }
    }

    return utils::getLastGLError();
}


}
