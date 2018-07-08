#ifndef MATH_ALGEBRA__MATRIX_H_
#define MATH_ALGEBRA__MATRIX_H_

#include <string>

#include "general.h"
#include "dynamic_array.h"


// -----------------------------------------------------------------------
// Forward Declerations

namespace math {

namespace algebra {

class Vector;

}

}

// Forward Declerations
// -----------------------------------------------------------------------


namespace math {

namespace algebra {

class Matrix
{
	util::DynamicArray<float, 2> m_data_;

	static void ValidateDimensions(int size);
	
public:

	// --------------------- Constructors and Factories ---------------------

	explicit Matrix(int size, bool init_values = false);

	explicit Matrix(int size, float** values);

	explicit Matrix(int size, float values, ...);

	explicit Matrix(int size, int values, ...);

	static Matrix MakeIdentity(int dimensions);

	static Matrix* MakeNewIdentity(int dimensions);

	// --------------------- Accessors --------------------- 

	int GetDimensions() const { return m_data_.GetSize(); }
	
	float& operator()(int row, int col);

	float operator()(int row, int col) const;

	float* operator()(int row);

	const float* operator()(int row) const;

	float& At(const int row, const int col) { return operator()(row, col); }

	float At(const int row, const int col) const { return operator()(row, col); }

	float* At(const int row) { return operator()(row); }

	const float* At(const int row) const { return operator()(row); }

	float* Base() { return m_data_.Base(); }

	const float* Base() const { return m_data_.Base(); }

	static Vector GetRow(const Matrix& mat, int nRow);

	static Vector GetColumn(const Matrix& mat, int nColumn);

	static void GetRow(const Matrix& mat, int nRow, Vector& row);

	static void GetColumn(const Matrix& mat, int nColumn, Vector& column);

	static void SetRow(Matrix& mat, int nRow, const Vector& row);

	static void SetColumn(Matrix& mat, int nColumn, const Vector& column);


	// --------------------- Matrix->Matrix Operations --------------------- 

	static void Add(const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Add(const Matrix& self, const Matrix& other)
	{
		Matrix ret(self.GetDimensions());
		Add(self, other, ret);
		return ret;
	}

	Matrix operator+(const Matrix& other) const { return Add(*this, other); }

	Matrix& operator+=(const Matrix& other) { Add(*this, other, *this); return *this; }

	static void Substract(const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Substract(const Matrix& self, const Matrix& other)
	{
		Matrix ret(self.GetDimensions());
		Substract(self, other, ret);
		return ret;
	}

	Matrix operator-(const Matrix& other) const { return Substract(*this, other); }

	Matrix& operator-=(const Matrix& other) { Substract(*this, other, *this); return *this; }

	static void Negate(const Matrix& self, Matrix& dest);

	static Matrix Negate(const Matrix& self)
	{
		Matrix ret(self.GetDimensions());
		Negate(self, ret);
		return ret;
	}

	Matrix operator-() const { return Negate(*this); }

	static void Multiply(const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Multiply(const Matrix& self, const Matrix& other)
	{
		Matrix ret(self.GetDimensions());
		Multiply(self, other, ret);
		return ret;
	}

	Matrix operator*(const Matrix& other) const { return Multiply(*this, other); }

	Matrix& operator*=(const Matrix& other);

	static bool Equals(const Matrix& self, const Matrix& other, float tolerance = util::kPercision);

	bool operator==(const Matrix& other) const { return Equals(*this, other); }

	bool operator!=(const Matrix& other) const { return !Equals(*this, other); }

	// --------------------- Matrix->Scalar Operations ---------------------
	static void Multiply(const Matrix& self, float scale, Matrix& dest);

	static Matrix Multiply(const Matrix& self, const float scale)
	{
		Matrix ret(self.GetDimensions());
		Multiply(self, scale, ret);
		return ret;
	}

	Matrix operator*(const float scale) const { return Multiply(*this, scale); }

	Matrix& operator*=(const float scale) { Multiply(*this, scale, *this); return *this; }

	static void Divide(const Matrix& self, float scale, Matrix& dest);

	static Matrix Divide(const Matrix& self, const float scale)
	{
		Matrix ret(self.GetDimensions());
		Divide(self, scale, ret);
		return ret;
	}
	
	Matrix operator/(const float scale) const { return Divide(*this, scale); }

	Matrix& operator/=(const float scale) { Divide(*this, scale, *this); return *this; }

	// --------------------- Matrix->Vector Operations ---------------------

	static Vector Multiply(const Matrix& self, const Vector& vec);

	static void Multiply(const Matrix& self, const Vector& vec, Vector& dest);

	Vector operator*(const Vector& vec) const;

	// --------------------- Properties ---------------------

	bool IsIdentity(float tolerance = util::kPercision) const;

	bool IsZero(float tolerance = util::kPercision) const;

	// --------------------- Helper Functions ---------------------

	static void SwapRows(Matrix& self, int i, int j);

	std::string ToString() const;

	bool IsValid() const;

};

}

}

#endif // MATH_ALGEBRA__MATRIX_H_
