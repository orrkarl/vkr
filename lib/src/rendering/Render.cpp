#include <algorithm>
#include <algorithm>
#include <cstring>
#include <memory>
#include <numeric>
#include <vector>
#include <unordered_map>

#include <rendering/Render.h>

#include <utils/converters.h>
#include <utils/memory.h>

namespace nr
{

Error Render::bind(const RenderData& obj, GLuint* vaoptr) const
{
    auto buffers = obj.getContent();

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

    glGenVertexArrays(1, vaoptr);
    auto vao = *vaoptr;
    glBindVertexArray(vao);

    GLuint d_buf;
    glGenBuffers(1, &d_buf);
    glBindBuffer(GL_ARRAY_BUFFER, d_buf);
    glBufferData(d_buf, h_size, h_buf, GL_STATIC_DRAW);

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
            return utils::fromGLError(err);
        }
    }

    return Error::NO_ERROR;
}


void Render::unbind(GLuint* vaoptr) const
{
    auto vao = *vaoptr;
    if (vao != 0)
    {
        GLint bufferAsInt;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bufferAsInt);
        glBindVertexArray(vao);
        GLuint buffer = (GLuint) bufferAsInt;
        
        glDeleteBuffers(1, &buffer);
        glDeleteVertexArrays(1, &vao);
        *vaoptr = 0;
    }
}

Error Render::link()
{
    glLinkProgram(m_program);
    
    GLint isLinked;
    glGetProgramiv(m_program, GL_LINK_STATUS, &isLinked);

    if (isLinked == GL_FALSE)
    {
        glDeleteProgram(m_program);
        return utils::getLastGLError();
    }

    glDetachShader(m_program, m_vertexShader->getContent());    
    glDetachShader(m_program, m_fragmentShader->getContent());

    return Error::NO_ERROR;
}

Error Render::drawArrays(const RenderData& obj) const
{
    GLuint vao;
    auto err = bind(obj, &vao);
    if (!error::isSuccess(err)) 
    {
        unbind(&vao);
        return err;
    }

    auto data = obj.getContent();

    if (data.count(0) == 0)
    {
        return Error::INVALID_VALUE;
    }

    auto vertBuf = obj.getContent().at(0);
    auto itemCount = vertBuf->getSize() / vertBuf->getElementSize();

    glUseProgram(m_program);
    glBindVertexArray(vao);
    
    glDrawArrays(utils::fromNRPrimitiveType(obj.getPrimitiveType()), 0, itemCount);
    
    glBindVertexArray(vao);
    glUseProgram(m_program);
    
    unbind(&vao);

    return Error::NO_ERROR;
}

}
