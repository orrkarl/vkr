#include "vector.h"
#include "strings.h"

namespace math {
	
namespace algebra {

#if _MATH_DEBUG_
#define VALIDATE_VECTOR_DIMENSION(vec, dim) COND_ASSERT(_MATH_DEBUG_, vec.m_data_.GetSize() == dim)
#else
#define VALIDATE_VECTOR_DIMENSION(mat, dim)
#endif

// -------------------------------------------------------------------------------------
// Constructors 
// -------------------------------------------------------------------------------------

#if _MATH_DEBUG_
Vector::Vector(const int dimension)
	: m_data_(dimension)
{
	ValidateDimension(dimension);
	for (auto i = 0; i < dimension; ++i)
		m_data_[i] = NAN;
}
#else
Vector::Vector(const int dimension)
	: m_data_(new float[dimension])
{
}
#endif // _MATH_DEBUG_

Vector::Vector(const int dimension, const float init_value) 
	: Vector(dimension)
{
	for (auto i = 0; i < dimension; ++i)
		m_data_[i] = init_value;
}


Vector::Vector(const int dimension, const float* values)
	: Vector(dimension)
{
	for (auto i = 0; i < dimension; ++i)
		m_data_[i] = values[i];
}

Vector::Vector(const int dimension, const float values, ...) 
	: Vector(dimension)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = values;
	for (auto i = 1; i < dimension; ++i)
		m_data_[i] = va_arg(va, float);

	va_end(va);
}

Vector::Vector(const int dimension, const int values, ...) 
	: Vector(dimension)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = float(values);
	for (auto i = 1; i < dimension; ++i)
		m_data_[i] = float(va_arg(va, int));

	va_end(va);
}




// -------------------------------------------------------------------------------------
// Accessors 
// -------------------------------------------------------------------------------------

float& Vector::At(const int index)
{
	COND_ASSERT(_MATH_DEBUG_, 0 <= index && index < m_data_.GetSize());

	return m_data_[index];
}

const float& Vector::At(const int index) const
{
	COND_ASSERT(_MATH_DEBUG_, 0 <= index && index < m_data_.GetSize());

	return m_data_[index];
}

float* Vector::Base()
{
#if _MATH_DEBUG_
	return m_data_.Base();
#else 
	return m_data_.get();
#endif // _MATH_DEBUG_
}

const float* Vector::Base() const
{
#if _MATH_DEBUG_
	return m_data_.Base();
#else 
	return m_data_.get();
#endif // _MATH_DEBUG_
}



// -------------------------------------------------------------------------------------
// Vector->Vector Operations 
// -------------------------------------------------------------------------------------

void Vector::Add(const int dimension, const Vector& self, const Vector& other, Vector& dest)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(other, dimension);
	VALIDATE_VECTOR_DIMENSION(dest, dimension);

	for (auto i = 0; i < dimension; ++i)
		dest.At(i) = self.m_data_[i] + other.m_data_[i];
}

Vector Vector::Add(const int dimension, const Vector& self, const Vector& other)
{
	Vector ret(dimension);
	Add(dimension, self, other, ret);
	return ret;
}

void Vector::Substract(const int dimension, const Vector& self, const Vector& other, Vector& dest)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(other, dimension);
	VALIDATE_VECTOR_DIMENSION(dest, dimension);

	for (auto i = 0; i < dimension; ++i)
		dest.At(i) = self.m_data_[i] - other.m_data_[i];
}

Vector Vector::Substract(const int dimension, const Vector& self, const Vector& other)
{
	Vector ret(dimension);
	Substract(dimension, self, other, ret);
	return ret;
}

void Vector::Negate(const int dimension, const Vector& self, Vector& dest)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(dest, dimension);

	for (auto i = 0; i < dimension; ++i)
		dest.At(i) = -self.m_data_[i];
}

Vector Vector::Negate(const int dimension, const Vector& self)
{
	Vector ret(dimension);
	Negate(dimension, self, ret);
	return ret;
}

bool Vector::Equals(const int dimension, const Vector& self, const Vector& other, const float tolerance)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(other, dimension);

	for (auto i = 0; i < dimension; ++i)
		if (!util::equals_rounded(self.m_data_[i], other.m_data_[i], tolerance))
			return false;

	return true;
}



// -------------------------------------------------------------------------------------
// Vector->Scalar Operations 
// -------------------------------------------------------------------------------------

void Vector::Multiply(const int dimension, const Vector& self, const float scale, Vector& dest)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(dest, dimension);

	for (auto i = 0; i < dimension; ++i)
		dest.At(i) = self.At(i) * scale;
}

Vector Vector::Multiply(const int dimension, const Vector& self, const float scale)
{
	Vector ret(dimension);
	Multiply(dimension, self, scale, ret);
	return ret;
}

void Vector::Divide(const int dimension, const Vector& self, const float scale, Vector& dest)
{
	VALIDATE_VECTOR_DIMENSION(self, dimension);
	VALIDATE_VECTOR_DIMENSION(dest, dimension);
	COND_ASSERT(_MATH_DEBUG_, !util::equals_rounded(scale, 0));

	for (auto i = 0; i < dimension; ++i)
		dest.At(i) = self.At(i) / scale;
}

Vector Vector::Divide(const int dimension, const Vector& self, const float scale)
{
	Vector ret(dimension);
	Divide(dimension, self, scale, ret);
	return ret;
}



// -------------------------------------------------------------------------------------
// Geometric Properties
// -------------------------------------------------------------------------------------

float Vector::DotProductSquared(const int dimension, const Vector& vec1, const Vector& vec2)
{
	VALIDATE_VECTOR_DIMENSION(vec1, dimension);
	VALIDATE_VECTOR_DIMENSION(vec2, dimension);

	float ret = 0;

	for (auto i = 0; i < dimension; ++i)
		ret += vec1.At(i) * vec2.At(i);

	return ret;
}



// -------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------

std::string Vector::ToString(const int dimension) const
{
	VALIDATE_VECTOR_DIMENSION((*this), dimension);

	std::string ret("Vector");
	ret += TO_STRING(dimension) += " [";
	
	for (auto i = 0; i < dimension - 1; ++i)
		ret += TO_STRING(m_data_[i]) += ", ";
	ret += TO_STRING(m_data_[dimension - 1]) += ']';
	
	return ret;
}

bool Vector::IsValid(const int dimension) const
{
	VALIDATE_VECTOR_DIMENSION((*this), dimension);

	for (auto i = 0; i < dimension; ++i)
		if (!isfinite(m_data_[i]))
			return false;

	return true;
}

}

}

