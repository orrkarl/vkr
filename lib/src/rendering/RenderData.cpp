#include <algorithm>
#include <cstring>
#include <memory>
#include <numeric>
#include <vector>
#include <unordered_map>

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
    std::transform(buffers.cbegin(), buffers.cend(), buffersMappedVec.begin(),
                     [](const std::pair<NRuint, Buffer*> in) { 
                         return in;
                      });


    std::vector<Buffer*> buffersVec;
    std::transform(buffersMappedVec.cbegin(), buffersMappedVec.cend(), buffersVec.begin(),
                     [](const std::pair<NRuint, Buffer*> in) { 
                         return in.second;
                      });

    
    auto unified = utils::unifyBuffers(buffersVec);
    auto h_buf = unified.second;
    auto h_size = unified.first;

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    if (!isBound())
    {
        glGenBuffers(1, &m_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferData(m_buffer, h_size, h_buf, GL_STATIC_DRAW);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, h_size, h_buf);
    }

    GLint err;
    for (const auto& buffer : buffersMappedVec)
    {
        glEnableVertexAttribArray(buffer.first);
        glVertexAttribPointer(
            buffer.first, 
            buffer.second->getElementSize(), 
            utils::fromNRType(buffer.second->getDataType()),
            GL_FALSE,
            0,
            0);
        
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