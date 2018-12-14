#pragma once

#include <vector>

#include "../general/predefs.h"


#define CTOR(DataType, dataTypeName) 										\
	Buffer(std::vector<DataType>& data, NRuint elementSize = 1)				\
		: Buffer(data.data(), Type::dataTypeName, data.size(), elementSize)	\
	{																		\
	}																		\
	\
	Buffer(DataType* data, NRuint size, NRuint elementSize = 1)				\
		: Buffer(data, Type::dataTypeName, size, elementSize)				\
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

	CTOR(NRdouble, DOUBLE)

	CTOR(NRfloat, FLOAT)

	CTOR(NRlong, LONG)

	CTOR(NRulong, ULONG)

	CTOR(NRint, INT)

	CTOR(NRuint, UINT)

	CTOR(NRshort, SHORT)

	CTOR(NRushort, USHORT)

	CTOR(NRbyte, BYTE)

	CTOR(NRubyte, UBYTE)

	CTOR(NRchar, CHAR)

	//CTOR(NRbool, BOOL) 	
	/*
	since vector<bool> is specialized to be a bitset, it doesn't contain a bool array laid
	flat in the memory - so it can not be converted to a buffer, and you have to use the
	more dangeorous raw pointer.
	*/

	Buffer(NRbool* data, NRuint size, NRuint elementSize = 1)				
		: Buffer(data, Type::BOOL, size, elementSize)				\
	{																		
	}


	void* getData() const { return m_data; }
	
	Type getDataType() const { return m_dataType; }
	
	NRuint getSize() const { return m_size; }

	NRuint getElementSize() const { return m_elementSize; }

	NRuint getByteSize() const { return getSize() * type::getByteSize(getDataType()); }

	void release() 
	{
		m_data = nullptr; 
	}
};

}

#undef CTOR
