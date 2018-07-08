#ifndef UTIL__DYNAMIC_ARRAY_H_
#define UTIL__DYNAMIC_ARRAY_H_
#include "error_handling.h"

namespace util {

template <typename T, const int Depth>
class DynamicArray
{
	T* m_values_;
	const int m_size_;

public:
	explicit DynamicArray(const int size, const bool init_values = false) 
		: m_values_(new T[ipow(size, Depth)]), m_size_(size)
	{
		if (init_values)
		{
			for (auto i = 0; i < GetActualSize(); ++i)
				m_values_[i] = T();
		}
	}

	DynamicArray(const int size, const float* values) : m_values_(new T[ipow(size, Depth)]), m_size_(size)
	{
		for (auto i = 0; i < m_size_; ++i)
			m_values_[i] = values[i];
	}

	~DynamicArray() { delete[] m_values_; }

	DynamicArray(const DynamicArray<T, Depth>& pOther)
		: m_values_(new T[pOther.GetActualSize()]),
		  m_size_(pOther.m_size_)
	{
		for (auto i = 0; i < pOther.GetActualSize(); ++i)
			m_values_[i] = pOther[i];
	}

	T& operator[](const int index)
	{
		ASSERT(0 <= index && index < GetActualSize()); 
		return m_values_[index];
	}

	const T& operator[](const int index) const
	{
		ASSERT(0 <= index && index < GetActualSize());
		return m_values_[index];
	}

	int GetSize() const { return m_size_; }

	T* Base() { return m_values_; }

	const T* Base() const { return m_values_; }

	static int GetDepth() { return Depth; }

	int GetActualSize() const { return ipow(GetSize(), GetDepth()); }
};

}

#endif // UTIL__DYNAMIC_ARRAY_H_
