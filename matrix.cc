#include "matrix.h"

#include <cstdarg>

#include "vector.h"
#include "error_handling.h"
#include "strings.h"

namespace math {
	
namespace algebra {

void Matrix::ValidateDimensions(const int size)
{
	ASSERT(size > 1);
}

int get_index(const int size, const int row, const int col)
{
	return size * row + col;
}

// -------------------------------------------------------------------------------------
// Constructors 
// -------------------------------------------------------------------------------------

Matrix::Matrix(const int size, const bool init_values) : m_data_(size, init_values)
{
	ValidateDimensions(size);
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

Matrix Matrix::MakeIdentity(const int dimensions)
{
	Matrix ret(dimensions);

	for (auto i = 0; i < dimensions; ++i)
		for (auto j = 0; j < dimensions; ++j)
			ret(i, j) = i == j ? 1 : 0;

	return ret;
}

Matrix* Matrix::MakeNewIdentity(const int dimensions)
{
	auto ret = new Matrix(dimensions);

	for (auto i = 0; i < dimensions; ++i)
		ret->At(i, i) = 1;

	return ret;
}


	
// -------------------------------------------------------------------------------------
// Accessors 
// -------------------------------------------------------------------------------------

float& Matrix::operator()(const int row, const int col)
{
	return m_data_[get_index(m_data_.GetSize(), row, col)];
}

float Matrix::operator()(const int row, const int col) const
{
	return m_data_[get_index(m_data_.GetSize(), row, col)];
}

const float* Matrix::operator()(const int row) const
{
	ASSERT(0 <= row && row < m_data_.GetSize());

	return m_data_.Base() + m_data_.GetSize() * row;
}


float* Matrix::operator()(const int row)
{
	ASSERT(0 <= row && row < m_data_.GetSize());

	return m_data_.Base() + m_data_.GetSize() * row;
}

Vector Matrix::GetRow(const Matrix& mat, const int nRow)
{
	return Vector(mat.GetDimensions(), mat(nRow));
}

Vector Matrix::GetColumn(const Matrix& mat, const int nColumn)
{
	Vector ret(mat.GetDimensions());

	for (auto i = 0; i <  mat.GetDimensions(); ++i)
		ret(i) = mat(nColumn, i);

	return ret;
}

void Matrix::GetRow(const Matrix& mat, const int nRow, Vector& row)
{
	for (auto i = 0; i < mat.GetDimensions(); ++i)
	{
		row(i) = mat(nRow, i);
	}
}

void Matrix::GetColumn(const Matrix& mat, const int nColumn, Vector& column)
{
	for (auto i = 0; i < mat.GetDimensions(); ++i)
	{
		column(i) = mat(i, nColumn);
	}
}

void Matrix::SetRow(Matrix& mat, const int nRow, const Vector& row)
{
	for (auto i = 0; i < mat.GetDimensions(); ++i)
	{
		mat(nRow, i) = row(i);
	}
}

void Matrix::SetColumn(Matrix& mat, const int nColumn, const Vector& column)
{
	for (auto i = 0; i < mat.GetDimensions(); ++i)
	{
		mat(i, nColumn) = column(i);
	}
}



// -------------------------------------------------------------------------------------
// Matrix->Matrix Operations 
// -------------------------------------------------------------------------------------

void Matrix::Add(const Matrix& self, const Matrix& other, Matrix& dest)
{
	ASSERT(self.GetDimensions() == other.GetDimensions());
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			dest(i, j) = self(i, j) + other(i, j);
}

void Matrix::Substract(const Matrix& self, const Matrix& other, Matrix& dest)
{
	ASSERT(self.GetDimensions() == other.GetDimensions());
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			dest(i, j) = self(i, j) - other(i, j);
}

void Matrix::Negate(const Matrix& self, Matrix& dest)
{
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			dest(i, j) = -self(i, j);
}

void Matrix::Multiply(const Matrix& self, const Matrix& other, Matrix& dest)
{
	ASSERT(self.GetDimensions() == other.GetDimensions());
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	float row_sum = 0;

	for (auto i = 0; i < self.GetDimensions(); ++i)
	{
		for (auto j = 0; j < self.GetDimensions(); ++j)
		{
			row_sum = 0;

			for (auto k = 0; k < self.GetDimensions(); ++k)
				row_sum += self(i, k) * other(k, j);

			dest(i, j) = row_sum;
		}
	}
}


Matrix& Matrix::operator*=(const Matrix& other)
{
	auto tmp = Multiply(*this, other);
	for (auto i = 0; i < other.GetDimensions() * other.GetDimensions(); ++i)
		m_data_[i] = other(i / other.GetDimensions(), i % other.GetDimensions());
	return *this;
}

bool Matrix::Equals(const Matrix& self, const Matrix& other, const float tolerance)
{
	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			if (!util::equals_rounded(self(i, j), other(i, j), tolerance))
				return false;

	return true;
}


// -------------------------------------------------------------------------------------
// Matrix->Scalar Operations 
// -------------------------------------------------------------------------------------

void Matrix::Multiply(const Matrix& self, const float scale, Matrix& dest)
{
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			dest(i, j) = scale * self(i, j);
}

void Matrix::Divide(const Matrix& self, const float scale, Matrix& dest)
{
	ASSERT(!util::equals_rounded(scale, 0));
	ASSERT(self.GetDimensions() == dest.GetDimensions());

	for (auto i = 0; i < self.GetDimensions(); ++i)
		for (auto j = 0; j < self.GetDimensions(); ++j)
			dest(i, j) = self(i, j) / scale;
}



// -------------------------------------------------------------------------------------
// Matrix->Vector Operations 
// -------------------------------------------------------------------------------------


Vector Matrix::Multiply(const Matrix& self, const Vector& vec)
{
	Vector ret(self.GetDimensions());
	Multiply(self, vec, ret);
	return ret;
}


void Matrix::Multiply(const Matrix& self , const Vector& vec, Vector& dest)
{
	ASSERT(self.GetDimensions() == dest.GetDimension());

	float row_sum = 0;

	for (auto i = 0; i < self.GetDimensions(); ++i)
	{
		for (auto j = 0; j < self.GetDimensions(); ++j)
			row_sum += self(i, j) *vec(j);

		dest(i) = row_sum;
		row_sum = 0;
	}
}

Vector Matrix::operator*(const Vector& vec) const
{
	return Multiply(*this, vec);
}



// -------------------------------------------------------------------------------------
// Properties 
// -------------------------------------------------------------------------------------

bool Matrix::IsIdentity(const float tolerance) const
{
	for (auto i = 0; i < m_data_.GetActualSize(); ++i)
	{
		if (!util::equals_rounded(m_data_[i], 1, tolerance) && i % GetDimensions() == i / GetDimensions())
			return false;

		if (!util::equals_rounded(m_data_[i], 0, tolerance) && i % GetDimensions() != i / GetDimensions())
			return false;
	}

	return true;
}

bool Matrix::IsZero(const float tolerance) const
{
	for (auto i = 0; i < m_data_.GetActualSize(); ++i)
		if (!util::equals_rounded(m_data_[i], 0, tolerance))
			return false;

	return true;
}



// -------------------------------------------------------------------------------------
// Helper Functions
// -------------------------------------------------------------------------------------

void Matrix::SwapRows(Matrix& self, const int i, const int j)
{
	ASSERT(0 <= i && i < self.GetDimensions());
	ASSERT(0 <= j && j < self.GetDimensions());

	float tmp = 0;

	for (auto index = 0; index < self.GetDimensions(); ++index)
	{
		tmp = self(i, index);
		self(i, index) = self(j, index);
		self(j, index) = tmp;
	}
}

std::string Matrix::ToString() const
{
	std::string ret;

	for (auto i = 0; i < GetDimensions(); ++i)
	{
		for (auto j = 0; j < GetDimensions(); ++j)
			ret += TO_STRING(At(i, j)) += " ";

		ret += "\n";
	}

	return ret;
}

bool Matrix::IsValid() const
{
	for (auto i = 0; i < m_data_.GetActualSize(); ++i)
		if (!isfinite(m_data_[i]))
			return false;

	return true;
}

}

}
