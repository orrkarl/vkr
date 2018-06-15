#ifndef MATH_ALGEBRA__VECTOR_H_
#define MATH_ALGEBRA__VECTOR_H_

#include <cstdarg>

#include "except.h"
#include "general_ops.h"
#include "strings.h"

namespace math {

namespace algebra {

template <const int Dimension>
class Vector
{
private:
	double m_values_[Dimension]{};

	static void __fastcall ValidateDimension(const int dimension)
	{
		THROW_IF(Dimension < 2, std::runtime_error("cannot create vector of dimension < 2"));
	}

public:
	Vector()
	{
		ValidateDimension(Dimension);
	}

	explicit Vector(double values...) : Vector()
	{
		va_list va;
		va_start(va, values);
		m_values_[0] = values;
		for (auto i = 1; i < Dimension; ++i)
		{
			m_values_[i] = va_arg(va, double);
		}
		va_end(va);
	}

	explicit Vector(int values...) : Vector()
	{
		va_list va;
		va_start(va, values);
		m_values_[0] = double(values);
		for (auto i = 1; i < Dimension; ++i)
		{
			m_values_[i] = double(va_arg(va, int));
		}
		va_end(va);
	}

	explicit Vector(const double values[]) : Vector()
	{
		for (auto i = 0; i < Dimension; ++i)
			m_values_[i] = values[i];
	}

	Vector(const Vector& vec) = default;

	Vector(Vector&& vec) = default;

	Vector<Dimension>& operator=(Vector<Dimension>&& vec) = default;

	Vector<Dimension>& operator=(const Vector<Dimension>& other) = default;

	~Vector() = default;

	static double DotProduct(const Vector<Dimension>& vec1, const Vector<Dimension>& vec2)
	{
		return sqrt(DotProductSquared(vec1, vec2));
	}

	static double DotProductSquared(const Vector<Dimension>& vec1, const Vector<Dimension>& vec2)
	{
		double ret = 0;
		for (auto i = 0; i < Dimension; ++i)
		{
			ret += vec1[i] * vec2[i];
		}

		return ret;
	}

	static Vector<Dimension>& VectorProduct(const Vector<Dimension>& vectors...)
	{
		return nullptr;
	}

	static int VectorDimension()
	{
		return Dimension;
	}

	double Norm() const
	{
		return DotProduct(*this, *this);
	}

	double NormSquared() const
	{
		return DotProductSquared(*this, *this);
	}

	bool IsOrthogonalTo(const Vector<Dimension>& other) const
	{
		return !DotProductSquared(*this, other);
	}

	double operator[](const int index) const
	{
		THROW_IF(index < 0 || index >= Dimension,
			util::OutOfRange::GetInstance("index out of range: %d", index));

		return m_values_[index];
	}

	double& operator[](const int index)
	{
		THROW_IF(index < 0 || index >= Dimension,
			util::OutOfRange::GetInstance("index out of range: %d", index));

		return m_values_[index];
	}

	Vector<Dimension> operator+(const Vector<Dimension>& other) const
	{
		Vector<Dimension> vec;

		for (auto i = 0; i < Dimension; i++)
			vec[i] = m_values_[i] + other[i];

		return vec;
	}

	Vector<Dimension> operator-(const Vector<Dimension>& other) const
	{
		Vector<Dimension> vec;

		for (auto i = 0; i < Dimension; i++)
			vec[i] = m_values_[i] - other[i];

		return vec;
	}

	Vector<Dimension> operator*(const double scale) const
	{
		Vector<Dimension> vec;

		for (auto i = 0; i < Dimension; i++)
			vec[i] = m_values_[i] * scale;

		return vec;
	}

	Vector<Dimension> operator/(const double scale) const
	{
		THROW_IF(scale == 0, util::ZeroDivisionError());

		Vector<Dimension> vec;

		for (auto i = 0; i < Dimension; i++)
			vec[i] = m_values_[i] / scale;

		return vec;
	}

	Vector<Dimension>& operator+=(const Vector<Dimension>& other)
	{
		for (auto i = 0; i < Dimension; i++)
			m_values_[i] += other[i];

		return *this;
	}

	Vector<Dimension>& operator-=(const Vector<Dimension>& other)
	{
		for (auto i = 0; i < Dimension; i++)
			m_values_[i] -= other[i];

		return *this;
	}

	Vector<Dimension>& operator*=(const double scale)
	{
		for (auto i = 0; i < Dimension; i++)
			m_values_[i] *= scale;

		return *this;
	}

	Vector<Dimension>& operator/=(const double scale)
	{
		THROW_IF(scale == 0, util::ZeroDivisionError());

		for (auto i = 0; i < Dimension; i++)
			m_values_[i] /= scale;

		return *this;
	}

	bool operator==(const Vector<Dimension>& other)
	{
		for (auto i = 0; i < Dimension; ++i)
			if (this[i] != other[i]) return false;

		return true;
	}

	bool operator!=(const Vector<Dimension>& other)
	{
		return !(this == other);
	}

	void Normalize()
	{
		this->operator/=(this->Norm());
	}

	Vector<Dimension> AsUnitVector() const
	{
		return this->operator/(this->Norm());
	}

	bool IsUnitVector() const
	{
		return round(this->Norm(), 10) == 1.0;
	}

	std::string ToString()
	{
		std::string ret;
		ret += "Vector [";
		for (auto i = 0; i < Dimension - 1; i++)
			ret += TO_STRING(m_values_[i]) += L",";
		ret += TO_STRING(m_values_[Dimension - 1]);
		ret += "]";
		return ret;
	}
};

}

}

#endif // MATH_ALGEBRA__VECTOR_H_