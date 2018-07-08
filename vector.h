#ifndef MATH_ALGEBRA__VECTOR_H_
#define MATH_ALGEBRA__VECTOR_H_

#include <cstdarg>
#include <string>

#include "error_handling.h"
#include "general.h"
#include "dynamic_array.h"

namespace math {

namespace algebra {

class Vector
{
	util::DynamicArray<float, 1> m_data_;

	static void ValidateLength(const int length)
	{
		ASSERT(length >= 2);
	}

public:

	// --------------------- Constructors ---------------------

	explicit Vector(int size, bool init_values = false);

	explicit Vector(int size, const float* values);

	explicit Vector(int size, float values, ...);

	explicit Vector(int size, int values, ...);

	// --------------------- Accessors --------------------- 

	int GetDimension() const { return m_data_.GetSize(); }

	float& operator()(const int index) { return m_data_[index]; }

	float operator()(const int index) const { return m_data_[index]; }

	float* Base() { return m_data_.Base(); }

	const float* Base() const { return m_data_.Base(); }

	// --------------------- Vector->Vector Operations --------------------- 

	static void Add(const Vector& self, const Vector& other, Vector& dest);
	
	static Vector Add(const Vector& self, const Vector& other)
	{
		Vector ret(self.GetDimension());
		Add(self, other, ret);
		return ret;
	}

	Vector operator+(const Vector& other) const { return Add(*this, other); }

	Vector& operator+=(const Vector& other) { Add(*this, other, *this); return *this; }

	static void Substract(const Vector& self, const Vector& other, Vector& dest);

	static Vector Substract(const Vector& self, const Vector& other)
	{
		Vector ret(self.GetDimension());
		Substract(self, other, ret);
		return ret;
	}

	Vector operator-(const Vector& other) const { return Substract(*this, other); }

	Vector& operator-=(const Vector& other) { Substract(*this, other, *this); return *this; }

	static void Negate(const Vector& self, Vector& dest);

	static Vector Negate(const Vector& self)
	{
		Vector ret(self.GetDimension());
		Negate(self, ret);
		return ret;
	}

	Vector operator-() const { return Negate(*this); }

	static bool Equals(const Vector& self, const Vector& other, float tolerance = util::kPercision);

	bool operator==(const Vector& other) const { return Equals(*this, other); }

	bool operator!=(const Vector& other) const { return !Equals(*this, other); }

	// --------------------- Vector->Scalar Operations --------------------- 

	static void Multiply(const Vector& self, float scale, Vector& dest);

	static Vector Multiply(const Vector& self, const float scale)
	{
		Vector ret(self.GetDimension());
		Multiply(self, scale, ret);
		return ret;
	}

	Vector operator*(const float scale) const { return Multiply(*this, scale); }

	Vector& operator*=(const float scale) { Multiply(*this, scale, *this); return *this; }

	static void Divide(const Vector& self, float scale, Vector& dest);

	static Vector Divide(const Vector& self, const float scale)
	{
		Vector ret(self.GetDimension());
		Divide(self, scale, ret);
		return ret;
	}

	Vector operator/(const float scale) const { return Divide(*this, scale); }

	Vector& operator/=(const float scale) { Divide(*this, scale, *this); return *this; }


	// --------------------- Geometric Properties --------------------- 

	static float DotProductSquared(const Vector& vec1, const Vector& vec2);

	static float DotProduct(const Vector& vec1, const Vector& vec2) { return sqrt(DotProductSquared(vec1, vec2)); }

	float GetLength() const { return DotProduct(*this, *this); }

	float GetLengthSquared() const { return DotProductSquared(*this, *this); }

	bool IsOrthogonalTo(const Vector& other) const { return DotProductSquared(*this, other) == 0; }

	float DistanceToSquared(const Vector& other) const { return (*this - other).GetLengthSquared(); }

	float DistanceTo(const Vector& other) const { return sqrt(DistanceToSquared(other)); }

	bool IsUnitVector(const float tolerance = util::kPercision) const { return util::equals_rounded(GetLengthSquared(), 1, tolerance); }

	bool IsZero(const float tolerance = util::kPercision) const { return util::equals_rounded(GetLengthSquared(), 0, tolerance); }

	// --------------------- Geometric Operations --------------------- 

	static void AsUnitVector(const Vector& self, Vector& dest) { return Divide(self, self.GetLength(), dest); }

	void Normalize() { AsUnitVector(*this, *this); }

	Vector AsUnitVector() const
	{
		Vector ret(GetDimension());
		AsUnitVector(*this, ret);
		return ret;
	}

	// --------------------- Helper Functions --------------------- 

	std::string ToString();

	bool IsValid() const;

};

}

}

#endif // MATH_ALGEBRA__VECTOR_H_