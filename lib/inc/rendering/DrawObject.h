#include <unordered_map>
#include <vector>

#include "../predefs.h"
#include "../Buffer.h"

#pragma once


namespace nr
{

// TODO: move the bindings thingie to Render, since render has to take the projection

class DrawObject
{
private:

	std::unordered_map<NRuint, Buffer*> m_buffers{};
	GLuint m_vao = 0;

public:

	~DrawObject()
	{
		resetBindings();
	}

	void bindBuffer(const NRuint index, Buffer* buffer)
	{
		m_buffers.insert({ index, buffer });
	}

	Error finalizeBindings();

	void resetBindings()
	{
		if (m_vao != 0)
		{
			GLint bufferAsInt;
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bufferAsInt);
			GLuint buffer = (GLuint) bufferAsInt;
			
			glDeleteBuffers(1, &buffer);
			glDeleteVertexArrays(1, &m_vao);
			m_vao = 0;
		}
	}

	void release()
	{
		resetBindings();
	}

	GLuint getContent()
	{
		return m_vao;
	}
};

}
