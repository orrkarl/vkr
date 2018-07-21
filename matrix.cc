#include "matrix.h"

#include <cstdarg>

#include "vector.h"
#include "error_handling.h"
#include "strings.h"

namespace math {
	
namespace algebra {

void Matrix::ValidateDimensions(const int dim)
{
	COND_ASSERT(_MATH_DEBUG_, dim > 1);
}

int get_index(const int dim, const int row, const int col)
{
	COND_ASSERT(_MATH_DEBUG_, 0 <= row && row < dim);
	COND_ASSERT(_MATH_DEBUG_, 0 <= col && col < dim);
	COND_ASSERT(_MATH_DEBUG_, dim > 1);

	return dim * row + col;
}

#if _MATH_DEBUG_
#define VALIDATE_MATRIX_DIMENSION(mat, dim) COND_ASSERT(_MATH_DEBUG_, mat.m_data_.GetSize() == dim)
#else
#define VALIDATE_MATRIX_DIMENSION(mat, dim)
#endif

// -------------------------------------------------------------------------------------
// Constructors 
// -------------------------------------------------------------------------------------

#if _MATH_DEBUG_
Matrix::Matrix(const int size) : m_data_(size)
{
	ValidateDimensions(size);
	for (auto i = 0; i < size * size; ++i)
		m_data_[i] = NAN;
}
#else
Matrix::Matrix(const int size) : m_data_(new float[size * size])
{
}
#endif // _DEBUG

Matrix::Matrix(const int size, const float initial_value) : Matrix(size)
{
	for (auto i = 0; i < size * size; ++i)
		m_data_[i] = initial_value;
}

Matrix::Matrix(const int size, float** values) : Matrix(size)
{
	for (auto i = 0; i < size; ++i)
		for (auto j = 0; j < size; ++j)
			m_data_[get_index(size, i, j)] = values[i][j];
}

Matrix::Matrix(const int size, float values, ...) : Matrix(size)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = values;
	for (auto i = 1; i < size * size; ++i)
		m_data_[i] = va_arg(va, float);

	va_end(va);
}

Matrix::Matrix(const int size, int values, ...) : Matrix(size)
{
	va_list va;
	va_start(va, values);

	m_data_[0] = values;
	for (auto i = 1; i < size * size; ++i)
		m_data_[i] = float(va_arg(va, int));

	va_end(va);
}

Matrix Matrix::MakeCopy(const int dimensions, const Matrix& mat)
{
	Matrix ret(dimensions);

	for (auto i = 0; i < dimensions * dimensions; ++i)
		ret.m_data_[i] = mat.m_data_[i];

	return ret;
}

Matrix* Matrix::MakeNewCopy(const int dimensions, const Matrix& mat)
{
	auto ret = new Matrix(dimensions);

	for (auto i = 0; i < dimensions * dimensions; ++i)
		ret->m_data_[i] = mat.m_data_[i];

	return ret;
}

Matrix Matrix::MakeIdentity(const int dimensions)
{
	Matrix ret(dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			ret.At(dimensions, i, j) = i == j ? 1 : 0;

	return ret;
}

Matrix* Matrix::MakeNewIdentity(const int dimensions)
{
	auto ret = new Matrix(dimensions);

	for (auto i = 0; i < dimensions; ++i)
		ret->At(dimensions, i, i) = 1;

	return ret;
}


	
// -------------------------------------------------------------------------------------
// Accessors 
// -------------------------------------------------------------------------------------

float& Matrix::At(const int dimensions, const int row, const int col)
{
	return m_data_[get_index(dimensions, row, col)];
}

float Matrix::At(const int dimensions, const int row, const int col) const
{
	return m_data_[get_index(dimensions, row, col)];
}

float* Matrix::At(const int dimensions, const int row)
{
	return Base() + get_index(dimensions, row, 0);
}

const float* Matrix::At(const int dimensions, const int row) const
{
	return Base() + get_index(dimensions, row, 0);
}

float* Matrix::Base()
{
#if _MATH_DEBUG_
	return m_data_.Base();
#else
	return m_data_.get();
#endif // _DEBUG
}

const float* Matrix::Base() const
{
#if _MATH_DEBUG_
	return m_data_.Base();
#else
	return m_data_.get();
#endif // _DEBUG
}

Vector Matrix::GetRow(const int dimensions, const Matrix& mat, const int nRow)
{
	VALIDATE_MATRIX_DIMENSION(mat, dimensions);
	return Vector(dimensions, mat.At(dimensions, nRow));
}

Vector Matrix::GetColumn(const int dimensions, const Matrix& mat, const int nColumn)
{
	Vector ret(dimensions);
	GetColumn(dimensions, mat, nColumn, ret);
	return ret;
}

void Matrix::GetRow(const int dimensions, const Matrix& mat, const int nRow, Vector& row)
{
	VALIDATE_MATRIX_DIMENSION(mat, dimensions);

	for (auto i = 0; i < dimensions; ++i)
	{
		row.At(i) = mat.At(dimensions, nRow, i);
	}
}

void Matrix::GetColumn(const int dimensions, const Matrix& mat, const int nColumn, Vector& column)
{
	VALIDATE_MATRIX_DIMENSION(mat, dimensions);

	for (auto i = 0; i < dimensions; ++i)
	{
		column.At(i) = mat.At(dimensions, i, nColumn);
	}
}

void Matrix::SetRow(const int dimensions, Matrix& mat, const int nRow, const Vector& row)
{
	VALIDATE_MATRIX_DIMENSION(mat, dimensions);

	for (auto i = 0; i < dimensions; ++i)
	{
		mat.At(dimensions, nRow, i) = row.At(i);
	}
}

void Matrix::SetColumn(const int dimensions, Matrix& mat, const int nColumn, const Vector& column)
{
	VALIDATE_MATRIX_DIMENSION(mat, dimensions);

	for (auto i = 0; i < dimensions; ++i)
	{
		mat.At(dimensions, i, nColumn) = column.At(i);
	}
}



// -------------------------------------------------------------------------------------
// Matrix->Matrix Operations 
// -------------------------------------------------------------------------------------

void Matrix::Add(const int dimensions, const Matrix& self, const Matrix& other, Matrix& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(other, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			dest.At(dimensions, i, j) = self.At(dimensions, i, j) + other.At(dimensions, i, j);
}

Matrix Matrix::Add(const int dimensions, const Matrix& self, const Matrix& other)
{
	Matrix ret(dimensions);
	Add(dimensions, self, other, ret);
	return ret;
}

void Matrix::Substract(const int dimensions, const Matrix& self, const Matrix& other, Matrix& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(other, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			dest.At(dimensions, i, j) = self.At(dimensions, i, j) - other.At(dimensions, i, j);
}

Matrix Matrix::Substract(const int dimensions, const Matrix& self, const Matrix& other)
{
	Matrix ret(dimensions);
	Substract(dimensions, self, other, ret);
	return ret;
}

void Matrix::Negate(const int dimensions, const Matrix& self, Matrix& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			dest.At(dimensions, i, j) = -self.At(dimensions, i, j);
}

Matrix Matrix::Negate(const int dimensions, const Matrix& self)
{
	Matrix ret(dimensions);
	Negate(dimensions, self, ret);
	return ret;
}

void Matrix::Multiply(const int dimensions, const Matrix& self, const Matrix& other, Matrix& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(other, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	float row_sum = 0;

	for (auto i = 0; i < dimensions; ++i)
	{
		for (auto j = 0; j < dimensions; ++j)
		{
			row_sum = 0;

			for (auto k = 0; k < dimensions; ++k)
				row_sum += self.At(dimensions, i, k) * other.At(dimensions, k, j);

			dest.At(dimensions, i, j) = row_sum;
		}
	}
}

Matrix Matrix::Multiply(const int dimensions, const Matrix& self, const Matrix& other)
{
	Matrix ret(dimensions);
	Multiply(dimensions, self, other, ret);
	return ret;
}

bool Matrix::Equals(const int dimensions, const Matrix& self, const Matrix& other, const float tolerance)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(other, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			if (!util::equals_rounded(self.At(dimensions, i, j), other.At(dimensions, i, j), tolerance))
				return false;

	return true;
}


// -------------------------------------------------------------------------------------
// Matrix->Scalar Operations 
// -------------------------------------------------------------------------------------

void Matrix::Multiply(const int dimensions, const Matrix& self, const float scale, Matrix& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			dest.At(dimensions, i, j) = scale * self.At(dimensions, i, j);
}

Matrix Matrix::Multiply(int dimensions, const Matrix& self, float scale)
{
	Matrix ret(dimensions);
	Multiply(dimensions, self, scale, ret);
	return ret;
}

void Matrix::Divide(const int dimensions, const Matrix& self, const float scale, Matrix& dest)
{
	COND_ASSERT(_MATH_DEBUG_, !util::equals_rounded(scale, 0));
	VALIDATE_MATRIX_DIMENSION(self, dimensions);
	VALIDATE_MATRIX_DIMENSION(dest, dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			dest.At(dimensions, i, j) = self.At(dimensions, i, j) / scale;
}

Matrix Matrix::Divide(const int dimensions, const Matrix& self, float scale)
{
	Matrix ret(dimensions);
	Divide(dimensions, self, scale, ret);
	return ret;
}



// -------------------------------------------------------------------------------------
// Matrix->Vector Operations 
// -------------------------------------------------------------------------------------

Vector Matrix::Multiply(const int dimensions, const Matrix& self, const Vector& vec)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);

	Vector ret(dimensions);
	Multiply(dimensions, self, vec, ret);
	return ret;
}


void Matrix::Multiply(const int dimensions, const Matrix& self , const Vector& vec, Vector& dest)
{
	VALIDATE_MATRIX_DIMENSION(self, dimensions);

	float row_sum = 0;

	for (auto i = 0; i < dimensions; ++i)
	{
		for (auto j = 0; j < dimensions; ++j)
			row_sum += self.At(dimensions, i, j) * vec.At(j);

		dest.At(i) = row_sum;
		row_sum = 0;
	}
}



// -------------------------------------------------------------------------------------
// Properties 
// -------------------------------------------------------------------------------------

bool Matrix::IsIdentity(const int dimensions, const float tolerance) const
{
	for (auto i = 0; i < dimensions * dimensions; ++i)
	{
		if (!util::equals_rounded(m_data_[i], 1, tolerance) && i % dimensions == i / dimensions)
			return false;

		if (!util::equals_rounded(m_data_[i], 0, tolerance) && i % dimensions != i / dimensions)
			return false;
	}

	return true;
}

bool Matrix::IsZero(const int dimensions, const float tolerance) const
{
	for (auto i = 0; i < dimensions * dimensions; ++i)
		if (!util::equals_rounded(m_data_[i], 0, tolerance))
			return false;

	return true;
}



// -------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------

void Matrix::SwapRows(const int dimensions, Matrix& self, const int i, const int j)
{
	COND_ASSERT(_MATH_DEBUG_, 0 <= i && i < dimensions);
	COND_ASSERT(_MATH_DEBUG_, 0 <= j && j < dimensions);
	VALIDATE_MATRIX_DIMENSION(self, dimensions);

	float tmp = 0;

	const auto tmp_row = new float[dimensions];

	memcpy_s(tmp_row, dimensions, self.At(dimensions, i), dimensions);
	memcpy_s(self.At(dimensions, i), dimensions, self.At(dimensions, j), dimensions);
	memcpy_s(self.At(dimensions, j), dimensions, tmp_row, dimensions);

	delete[] tmp_row;
}

std::string Matrix::ToString(const int dimensions) const
{
	std::string ret;

	for (auto i = 0; i < dimensions; ++i)
	{
		for (auto j = 0; j < dimensions; ++j)
			ret += TO_STRING(this->At(dimensions, i, j)) += " ";

		ret += "\n";
	}

	return ret;
}

bool Matrix::IsValid(const int dimensions) const
{
	for (auto i = 0; i < dimensions * dimensions; ++i)
		if (!isfinite(m_data_[i]))
			return false;

	return true;
}

}

}
