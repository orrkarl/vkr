#include <general/predefs.h>

#include <rendering/Render.h>
#include <utils/converters.h>

namespace nr
{

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

Error Render::drawArrays(RenderData& obj) const
{
    Error err = obj.finalizeBindings();
    
    if (!error::isSuccess(err)) 
    {
        obj.unbind();
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
    glDrawArrays(utils::fromNRPrimitiveType(obj.getPrimitiveType()), 0, itemCount);

    return Error::NO_ERROR;
}

}
