/*
 * Dynamically allocated array. Size is set on runtime, Depth (how many parts of the same size are there) on compile time.
 * Will ofc delete the contained array when discarded.
 */


#ifndef UTIL__DYNAMIC_ARRAY_H_
#define UTIL__DYNAMIC_ARRAY_H_
#include "error_handling.h"

namespace util {

template <typename T, const int Depth>
class DynamicArray
{
	std::unique_ptr<T[]> m_values_;
	const int m_size_;

public:
	// Will create an array of size 'size ** Depth'. if init_values it true, the array values will be default-initialized.
	explicit DynamicArray(const int size) 
		: m_values_(new T[ipow(size, Depth)]), m_size_(size)
	{
	}

	DynamicArray(const int size, const T initial_value)
		: m_values_(new T[ipow(size, Depth)]), m_size_(size)
	{
		for (auto i = 0; i < GetActualSize(); ++i)
			m_values_[i] = initial_value;
	}

	DynamicArray(const int size, const float* values) : m_values_(new T[ipow(size, Depth)]), m_size_(size)
	{
		for (auto i = 0; i < m_size_; ++i)
			m_values_[i] = values[i];
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

	// Returns the size of each array segment
	int GetSize() const { return m_size_; }

	// Pointer to the array
	T* Base() { return m_values_.get(); }

	// Pointer to the array
	const T* Base() const { return m_values_.get(); }

	static int GetDepth() { return Depth; }

	// Returns the actual (size ** depth) amount of cells this array holds
	int GetActualSize() const
	{
		if constexpr (Depth == 1) return m_size_;
		else if (Depth == 2) return m_size_ * m_size_;
		else if (Depth == 3) return m_size_ * m_size_ * m_size_;
		else return ipow(GetSize(), GetDepth());
	}
};

}

#endif // UTIL__DYNAMIC_ARRAY_H_
