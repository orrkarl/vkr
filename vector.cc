#include "vector.h"
#include "strings.h"

namespace math {
	
namespace algebra {

// -------------------------------------------------------------------------------------
// Constructors 
// -------------------------------------------------------------------------------------



Vector::Vector(const int size, const bool init_values) : m_data_(size, init_values) {}

Vector::Vector(const int size, const float* values) : m_data_(size, values) {}

Vector::Vector(const int size, const float values, ...) : Vector(size)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = values;
	for (auto i = 1; i < size; ++i)
		m_data_[i] = va_arg(va, float);

	va_end(va);
}

Vector::Vector(const int size, const int values, ...) : Vector(size)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = float(values);
	for (auto i = 1; i < size; ++i)
		m_data_[i] = float(va_arg(va, int));

	va_end(va);
}



// -------------------------------------------------------------------------------------
// Vector->Vector Operations 
// -------------------------------------------------------------------------------------

void Vector::Add(const Vector& self, const Vector& other, Vector& dest)
{
	ASSERT(self.GetDimension() == other.GetDimension());
	ASSERT(self.GetDimension() == dest.GetDimension());

	for (auto i = 0; i < self.GetDimension(); ++i)
		dest(i) = self.m_data_[i] + other.m_data_[i];
}

void Vector::Substract(const Vector& self, const Vector& other, Vector& dest)
{
	ASSERT(self.GetDimension() == other.GetDimension());
	ASSERT(self.GetDimension() == dest.GetDimension());

	for (auto i = 0; i < self.GetDimension(); ++i)
		dest(i) = self.m_data_[i] - other.m_data_[i];
}

void Vector::Negate(const Vector& self, Vector& dest)
{
	ASSERT(self.GetDimension() == dest.GetDimension());

	for (auto i = 0; i < self.GetDimension(); ++i)
		dest(i) = -self.m_data_[i];
}

bool Vector::Equals(const Vector& self, const Vector& other, const float tolerance)
{
	ASSERT(self.GetDimension() == other.GetDimension());

	for (auto i = 0; i < self.GetDimension(); ++i)
		if (!util::equals_rounded(self.m_data_[i], other.m_data_[i], tolerance))
			return false;

	return true;
}



// -------------------------------------------------------------------------------------
// Vector->Scalar Operations 
// -------------------------------------------------------------------------------------

void Vector::Multiply(const Vector& self, const float scale, Vector& dest)
{
	ASSERT(self.GetDimension() == dest.GetDimension());

	for (auto i = 0; i < self.GetDimension(); ++i)
		dest(i) = self(i) * scale;
}

void Vector::Divide(const Vector& self, const float scale, Vector& dest)
{
	ASSERT(self.GetDimension() == dest.GetDimension());
	ASSERT(!util::equals_rounded(scale, 0));

	for (auto i = 0; i < self.GetDimension(); ++i)
		dest(i) = self(i) / scale;
}



// -------------------------------------------------------------------------------------
// Geometric Properties
// -------------------------------------------------------------------------------------

float Vector::DotProductSquared(const Vector& vec1, const Vector& vec2)
{
	ASSERT(vec1.GetDimension() == vec2.GetDimension());

	float ret = 0;

	for (auto i = 0; i < vec1.GetDimension(); ++i)
		ret += vec1(i) * vec2(i);

	return ret;
}



// -------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------

std::string Vector::ToString()
{
	std::string ret("Vector");
	ret += TO_STRING(m_data_.GetSize()) += " [";
	for (auto i = 0; i < GetDimension() - 1; ++i)
		ret += TO_STRING(m_data_[i]) += ", ";
	ret += TO_STRING(m_data_[GetDimension() - 1]) += ']';
	return ret;
}

bool Vector::IsValid() const
{
	for (auto i = 0; i < m_data_.GetActualSize(); ++i)
		if (!isfinite(m_data_[i]))
			return false;

	return true;
}

}

}

