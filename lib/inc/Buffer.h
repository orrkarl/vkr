#include <vector>

#include "predefs.h"

#pragma once


#define CTOR(DataType, dataTypeName) 										\
	Buffer(std::vector<DataType>& data, NRuint elementSize = 1)				\
		: Buffer(&data[0], Type::dataTypeName, data.size(), elementSize)	\
	{																		\
	}


namespace nr
{

class Buffer
{
private:
	Type m_dataType;
	NRuint m_size;
	NRuint m_elementSize;
	void* m_data;
	
	Buffer(void* data, Type dataType, NRuint size, NRuint elementSize)
		: m_data(data), m_dataType(dataType), m_size(size), m_elementSize(elementSize)
	{
	}

public:

	CTOR(NRfloat, FLOAT)

	CTOR(NRint, INT)

	CTOR(NRuint, UINT)

	CTOR(NRchar, CHAR)

	void* getData() const { return m_data; }
	
	Type getDataType() const { return m_dataType; }
	
	NRuint getSize() const { return m_size; }

	NRuint getElementSize() const { return m_elementSize; }

	void release() 
	{
		m_data = nullptr; 
	}
};

}

#undef CTOR
