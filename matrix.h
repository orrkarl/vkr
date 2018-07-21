/**
 * Main mathematical matrix class.
 * Only supplies the general and basic operations.
 */

#ifndef MATH_ALGEBRA__MATRIX_H_
#define MATH_ALGEBRA__MATRIX_H_

#include <string>

#include "predefs.h"
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
#if _MATH_DEBUG_
	util::DynamicArray<float, 2> m_data_;
#else
	std::unique_ptr<float[]> m_data_;
#endif // _MATH_DEBUG_

	static void ValidateDimensions(int dim);
	
public:

	// --------------------- Constructors and Factories ---------------------

	explicit Matrix(int size);

	explicit Matrix(int size, float initial_value);

	explicit Matrix(int size, float** values);

	explicit Matrix(int size, float values, ...);

	explicit Matrix(int size, int values, ...);

	static Matrix MakeCopy(int dimensions, const Matrix& mat);

	static Matrix* MakeNewCopy(int dimensions, const Matrix& mat);
	
	static Matrix MakeIdentity(int dimensions);

	static Matrix* MakeNewIdentity(int dimensions);

	// --------------------- Accessors --------------------- 

	float& At(int dimensions, int row, const int col);

	float At(int dimensions, int row, const int col) const;

	float* At(int dimensions, int row);

	const float* At(int dimensions, int row) const;

	float* Base();

	const float* Base() const;

	static Vector GetRow(int dimensions, const Matrix& mat, int nRow);

	static Vector GetColumn(int dimensions, const Matrix& mat, int nColumn);

	static void GetRow(int dimensions, const Matrix& mat, int nRow, Vector& row);

	static void GetColumn(int dimensions, const Matrix& mat, int nColumn, Vector& column);

	static void SetRow(int dimensions, Matrix& mat, int nRow, const Vector& row);

	static void SetColumn(int dimensions, Matrix& mat, int nColumn, const Vector& column);


	// --------------------- Matrix->Matrix Operations --------------------- 

	static void Add(int dimensions, const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Add(int dimensions, const Matrix& self, const Matrix& other);

	static void Substract(int dimensions, const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Substract(int dimensions, const Matrix& self, const Matrix& other);

	static void Negate(int dimensions, const Matrix& self, Matrix& dest);

	static Matrix Negate(int dimensions, const Matrix& self);

	static void Multiply(int dimensions, const Matrix& self, const Matrix& other, Matrix& dest);

	static Matrix Multiply(int dimensions, const Matrix& self, const Matrix& other);

	static bool Equals(int dimensions, const Matrix& self, const Matrix& other, float tolerance = util::kPercision);

	// --------------------- Matrix->Scalar Operations ---------------------
	static void Multiply(int dimensions, const Matrix& self, float scale, Matrix& dest);

	static Matrix Multiply(int dimensions, const Matrix& self, float scale);

	static void Divide(int dimensions, const Matrix& self, float scale, Matrix& dest);

	static Matrix Divide(int dimensions, const Matrix& self, float scale);

	// --------------------- Matrix->Vector Operations ---------------------

	static Vector Multiply(int dimensions, const Matrix& self, const Vector& vec);

	static void Multiply(int dimensions, const Matrix& self, const Vector& vec, Vector& dest);

	// --------------------- Properties ---------------------

	bool IsIdentity(int dimensions, float tolerance = util::kPercision) const;

	bool IsZero(int dimensions, float tolerance = util::kPercision) const;

	// --------------------- Helper Functions ---------------------

	static void SwapRows(int dimensions, Matrix& self, int i, int j);

	std::string ToString(int dimensions) const;

	bool IsValid(int dimensions) const;

};

}

}

#endif // MATH_ALGEBRA__MATRIX_H_
