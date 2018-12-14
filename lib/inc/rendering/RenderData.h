#pragma once

#include <unordered_map>
#include <vector>

#include "../general/predefs.h"
#include "../buffers/Buffer.h"

namespace nr
{

class RenderData
{
private:
	std::unordered_map<NRuint, Buffer*> m_buffers{};
	const Primitive m_primitiveType;
	
	GLuint m_vao;
	GLuint m_buffer;

public:

	RenderData(const Primitive& type)
		: m_primitiveType(type)
	{
	}

	void bindBuffer(const NRuint index, Buffer* buffer)
	{
		m_buffers.insert({ index, buffer });
	}

	std::unordered_map<NRuint, Buffer*> getContent() const
	{
		return m_buffers;
	}

	Primitive getPrimitiveType() const { return m_primitiveType; }

	Error finalizeBindings();

	NRbool isBound() const { return m_vao == 0; }

    void unbind();
};

}
