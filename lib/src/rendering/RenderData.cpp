#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

#include <general/predefs.h>

#include <rendering/RenderData.h>

#include <utils/converters.h>
#include <utils/memory.h>


namespace nr
{

Error RenderData::initBindings()
{
    if (isBound())
    {
        return Error::NO_ERROR;
    }
    
    glGenVertexArrays(1, &m_vao);
    printf("vao: %x\n", m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_attrBuffer);
    glGenBuffers(1, &m_uniBuffer);

    return Error::NO_ERROR;
}

Error RenderData::finalizeAttributes()
{
    auto attrs = m_attributes;
    attrs.insert({ 0u, m_vertexBuffer });
    
    std::vector<std::pair<NRuint, Buffer*>> attrsMappedVec;
    std::transform(attrs.cbegin(), attrs.cend(), std::back_inserter(attrsMappedVec),
                     [](const std::pair<NRuint, Buffer*>& in) { 
                         return in;
                      });


    std::vector<Buffer*> attrsVec;
    std::transform(attrsMappedVec.cbegin(), attrsMappedVec.cend(), std::back_inserter(attrsVec),
                     [](const std::pair<NRuint, Buffer*>& in) { 
                         return in.second;
                      });

    
    auto unified = utils::unifyBuffers(attrsVec);
    auto h_buf = unified.second;
    auto h_size = unified.first;
    GLenum err;

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_attrBuffer);
    glBufferData(GL_ARRAY_BUFFER, h_size, h_buf, GL_STATIC_DRAW);

    if ((err = glGetError()) != GL_NO_ERROR)
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return utils::fromGLError(err);
    }

    NRuint offset = 0;
    for (const auto& buffer : attrsMappedVec)
    {
        glEnableVertexAttribArray(buffer.first);
        glVertexAttribPointer(
            buffer.first, 
            buffer.second->getElementSize(), 
            utils::fromNRType(buffer.second->getDataType()),
            GL_FALSE,
            0,
            (void*) offset);

        offset += buffer.second->getByteSize();

        if ((err = glGetError()) != GL_NO_ERROR)
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return utils::fromGLError(err);
        }
    }

    return Error::NO_ERROR;
}

Error RenderData::finalizeUniforms()
{
    NRuint index;
    Uniform* uniform;
    for (const auto unif : m_uniforms)
    {
        index = unif.first;
        uniform = unif.second;
        
        auto dim = uniform->getDimensions();

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
                                glUniformMatrix2fv(index, 4, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix2x3fv(index, 6, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix2x4fv(index, 8, GL_FALSE, data);
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
                                glUniformMatrix3x2fv(index, 6, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix3fv(index, 9, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix3x4fv(index, 12, GL_FALSE, data);
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
                                glUniformMatrix4x2fv(index, 8, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 3u:
                                glUniformMatrix4x3fv(index, 12, GL_FALSE, data);
                                return Error::NO_ERROR;
                            case 4u:
                                glUniformMatrix4fv(index, 16, GL_FALSE, data);
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

Error RenderData::finalizeBindings()
{
    Error err;
    if (error::isFailure((err = finalizeAttributes()))) return err;
    if (error::isFailure((err = finalizeUniforms()))) return err;
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniBuffer);

    return Error::NO_ERROR;
}

void RenderData::unbind()
{
    if (m_vao != 0)
    {       
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }

    if (m_attrBuffer != 0)
    {     
        glDeleteBuffers(1, &m_attrBuffer);
        m_attrBuffer = 0;
    }

    if (m_uniBuffer)
    {
        glDeleteBuffers(1, &m_uniBuffer);
        m_uniBuffer = 0;
    }
}

}