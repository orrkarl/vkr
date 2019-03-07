#pragma once

#include <map>
#include <set>

#include "../general/predefs.h"
#include "../buffers/Buffer.h"
#include "../buffers/Uniform.h"

namespace nr
{

class NR_SHARED_EXPORT RenderData
{

public:

	RenderData(const Primitive& type)
		: m_primitiveType(type)
	{
		initBindings();
	}

	Error bindAttribute(const NRuint index, Buffer* buffer)
	{
		if (buffer->getDataType() == Type::RAW) return Error::INVALID_TYPE;
		if (m_attributes.count(index) != 0)
		{
			if (buffer == nullptr)
			{
				m_attributes.erase(index);
				requestUpdate();
				return Error::NO_ERROR;
			}
			else
			{
				m_attributes.insert({ index, buffer });
				requestUpdate();
				return Error::NO_ERROR;
			}
		}
		if (isIndexBound(index))
		{
			return Error::INVALID_OPERATION;
		}

		m_attributes.insert({ index, buffer });
		requestUpdate();
		return Error::NO_ERROR;
	}

	void bindVertexBuffer(Buffer* buffer)
	{
		m_vertexBuffer = buffer;
		requestUpdate();
	}

	void requestUpdate() { m_hasUpdates = true; }

	Primitive getPrimitiveType() const { return m_primitiveType; }

	Error finalizeBindings();

	NRbool isBound() const { return m_vao != 0; }

	Buffer* getVertexBuffer() { return m_vertexBuffer; };

    void unbind();

private:
	std::map<NRuint, Buffer*> m_attributes{};

	Buffer* m_vertexBuffer;

	const Primitive m_primitiveType;
	
	GLuint m_vao = 0u;
	GLuint m_attrBuffer = 0u;

	NRbool m_hasUpdates = false;

	NRbool isIndexBound(const NRuint index) const
	{
		return index == 0u || m_attributes.count(index) != 0;
	}

	Error initBindings();

	Error finalizeAttributes();

	NRbool isUpToDate() const { return !m_hasUpdates; }

	void update() { m_hasUpdates = false; }

};

}
