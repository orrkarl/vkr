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

Error RenderData::finalizeBindings()
{
    auto buffers = getContent();
    
    std::vector<std::pair<NRuint, Buffer*>> buffersMappedVec;
    std::transform(buffers.cbegin(), buffers.cend(), std::back_inserter(buffersMappedVec),
                     [](const std::pair<NRuint, Buffer*>& in) { 
                         return in;
                      });


    std::vector<Buffer*> buffersVec;
    std::transform(buffersMappedVec.cbegin(), buffersMappedVec.cend(), std::back_inserter(buffersVec),
                     [](const std::pair<NRuint, Buffer*>& in) { 
                         return in.second;
                      });

    
    auto unified = utils::unifyBuffers(buffersVec);
    auto h_buf = unified.second;
    auto h_size = unified.first;
    GLenum err;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (!isBound())
    {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferData(GL_ARRAY_BUFFER, h_size, h_buf, GL_STATIC_DRAW);

        if ((err = glGetError()) != GL_NO_ERROR)
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return utils::fromGLError(err);
        }
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, h_size, h_buf);

        if ((err = glGetError()) != GL_NO_ERROR)
        {
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            return utils::fromGLError(err);
        }
    }


    NRuint offset = 0;
    for (const auto& buffer : buffersMappedVec)
    {
        glEnableVertexAttribArray(buffer.first);
      /*  std::cout << "index: " << buffer.first << ", elem size: " << buffer.second->getElementSize() << ", offset: " << offset << ", size: " << buffer.second->getSize() << std::endl;
        std::cout << "values: " << std::endl;
        auto data = (NRfloat*) buffer.second->getData();
        for (NRuint idx = 0; idx < buffer.second->getSize() / buffer.second->getElementSize(); ++idx)
        {
            std::cout << "\t[ ";
            for (auto j = 0; j < buffer.second->getElementSize(); ++j)
            {
                std::cout << data[idx * buffer.second->getElementSize() + j] << ' ';
            }
            std::cout << "]" << std::endl;
        }*/
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


void RenderData::unbind()
{
    if (m_vao != 0)
    {        
        glDeleteBuffers(1, &m_buffer);
        m_buffer = 0;

        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
}

}