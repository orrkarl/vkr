#ifndef MATH_ALGEBRA__VECTOR_H_
#define MATH_ALGEBRA__VECTOR_H_

#include <cstdarg>
#include <string>

#include "predefs.h"
#include "error_handling.h"
#include "general.h"
#include "dynamic_array.h"

namespace math {

namespace algebra {

// Mathematical vector class
class Vector
{
#if _MATH_DEBUG_
	util::DynamicArray<float, 1> m_data_;
#else
	std::unique_ptr<float[]> m_data_;
#endif // _MATH_DEBUG_

	static void ValidateDimension(const int length)
	{
		COND_ASSERT(_MATH_DEBUG_, length > 1);
	}

public:

	// --------------------- Constructors ---------------------

	explicit Vector(int dimension);

	explicit Vector(int dimension, float init_value);

	explicit Vector(int dimension, const float* values);

	explicit Vector(int dimension, float values, ...);

	explicit Vector(int dimension, int values, ...);

	// --------------------- Accessors --------------------- 

	float& At(int index);

	const float& At(int index) const;

	float* Base();

	const float* Base() const;

	// --------------------- Vector->Vector Operations --------------------- 

	static void Add(int dimension, const Vector& self, const Vector& other, Vector& dest);
	
	static Vector Add(int dimension, const Vector& self, const Vector& other);

	static void Substract(int dimension, const Vector& self, const Vector& other, Vector& dest);

	static Vector Substract(int dimension, const Vector& self, const Vector& other);

	static void Negate(int dimension, const Vector& self, Vector& dest);

	static Vector Negate(int dimension, const Vector& self);

	static bool Equals(int dimension, const Vector& self, const Vector& other, float tolerance = util::kPercision);

	// --------------------- Vector->Scalar Operations --------------------- 

	static void Multiply(int dimension, const Vector& self, float scale, Vector& dest);

	static Vector Multiply(int dimension, const Vector& self, float scale);

	static void Divide(int dimension, const Vector& self, float scale, Vector& dest);

	static Vector Divide(int dimension, const Vector& self, float scale);

	// --------------------- Geometric Properties --------------------- 

	static float DotProductSquared(int dimension, const Vector& vec1, const Vector& vec2);

	static float DotProduct(const int dimension, const Vector& vec1, const Vector& vec2)
	{
		return sqrt(DotProductSquared(dimension, vec1, vec2));
	}

	float Norm(const int dimension) const { return DotProduct(dimension, *this, *this); }

	float NormSquared(const int dimension) const { return DotProductSquared(dimension, *this, *this); }

	bool IsOrthogonalTo(const int dimension, const Vector& other) const { return DotProductSquared(dimension, *this, other) == 0; }

	float DistanceToSquared(const int dimension, const Vector& other) const
	{
		return Substract(dimension, *this, other).NormSquared(dimension);
	}

	float DistanceTo(const int dimension, const Vector& other) const { return sqrt(DistanceToSquared(dimension, other)); }

	bool IsUnitVector(const int dimension, const float tolerance = util::kPercision) const
	{
		return util::equals_rounded(NormSquared(dimension), 1, tolerance);
	}

	bool IsZero(const int dimension, const float tolerance = util::kPercision) const
	{
		return util::equals_rounded(NormSquared(dimension), 0, tolerance);
	}

	// --------------------- Geometric Operations --------------------- 

	static void AsUnitVector(const int dimension, const Vector& self, Vector& dest)
	{
		return Divide(dimension, self, self.Norm(dimension), dest);
	}

	void Normalize(const int dimension) { AsUnitVector(dimension, *this, *this); }

	Vector AsUnitVector(const int dimension) const
	{
		Vector ret(dimension);
		AsUnitVector(dimension, *this, ret);
		return ret;
	}

	// --------------------- Helper Functions --------------------- 

	std::string ToString(int dimension) const;

	bool IsValid(int dimension) const;

};

}

}

#endif // MATH_ALGEBRA__VECTOR_H_