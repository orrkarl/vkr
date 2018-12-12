#include <algorithm>
#include <numeric>
#include <memory>
#include <cstring>

#include "../inc/rendering/DrawObject.h"

#include "../inc/Converters.h"

namespace nr
{

Error DrawObject::finalizeBindings()
{
    resetBindings();

    NRuint totalSize = 0;

    for (const auto& it : m_buffers)
    {
        totalSize += it.second->getSize();
    }

    // actually using malloc in cpp, almost ashamed of myself...
    auto h_buf = std::make_unique<void*>(malloc(totalSize));
    totalSize = 0;

    for (const auto& buffer : m_buffers)
    {
        std::memcpy(h_buf.get() + totalSize, buffer.second->getData(), buffer.second->getSize());
        totalSize += buffer.second->getSize();
    }

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    GLuint d_buf;
    glGenBuffers(1, &d_buf);
    glBindBuffer(GL_ARRAY_BUFFER, d_buf);
    glBufferData(d_buf, totalSize, h_buf.get(), GL_STATIC_DRAW);

    GLint err;
    for (const auto& buffer : m_buffers)
    {
        glEnableVertexAttribArray(buffer.first);
        glVertexAttribPointer(
            buffer.first, 
            buffer.second->getElementSize(), 
            fromNRType(buffer.second->getDataType()),
            GL_FALSE,
            0,
            0);
        
        if ((err = glGetError()) != GL_NO_ERROR)
        {
            resetBindings();
            return fromGLError(err);
        }
    }

    glBindVertexArray(0);
    return Error::NO_ERROR;
}

}
