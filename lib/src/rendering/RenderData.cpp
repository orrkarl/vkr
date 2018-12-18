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
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_attrBuffer);

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
    Error err;

    glBindBuffer(GL_ARRAY_BUFFER, m_attrBuffer);
    glBufferData(GL_ARRAY_BUFFER, h_size, h_buf, GL_STATIC_DRAW);

    if (error::isFailure(err = utils::getLastGLError()))
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return err;
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
        if (error::isFailure(err = utils::getLastGLError()))
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return err;
        }
    }

    return Error::NO_ERROR;
}

Error RenderData::finalizeBindings()
{
    if (!isUpToDate())
    {
        update();
        glBindVertexArray(m_vao);
        return finalizeAttributes();
    }
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
}

}