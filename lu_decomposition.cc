#include "lu_decomposition.h"

#include "matrix.h"
#include "vector.h"


namespace math {
	
namespace algebra {


LUDecomposition::LUDecomposition(const int dimensions, std::unique_ptr<Matrix> mat)
	: m_decompositioned_matrix_(std::move(mat)),
	  m_permutations_vector_(new int[dimensions + 1]),
	  m_dimensions_(dimensions)
{
	for (auto i = 0; i <= m_dimensions_; ++i)
		m_permutations_vector_[i] = i;

	if (!Decompose()) Cleanup();
}

LUDecomposition::LUDecomposition(const int dimensions, const Matrix& mat)
	: LUDecomposition(dimensions, std::unique_ptr<Matrix>(Matrix::MakeNewCopy(dimensions, mat)))
{
}

void LUDecomposition::Cleanup()
{
	m_permutations_vector_.reset();
	m_decompositioned_matrix_.reset();
}


bool LUDecomposition::Decompose() const
{
	int j, k;
	float current_absolute_value;

	for (auto i = 0; i < m_dimensions_; ++i)
	{
		float column_max = 0;
		auto column_max_index = i;

		for (k = i; k < m_dimensions_; ++k)
		{
			if ((current_absolute_value = abs(m_decompositioned_matrix_->At(m_dimensions_, k, i))) > column_max)
			{
				column_max = current_absolute_value;
				column_max_index = k;
			}
		}

		if (column_max < util::kPercision)
			return false;

		if (column_max_index != i)
		{
			j = m_permutations_vector_[i];
			m_permutations_vector_[i] = m_permutations_vector_[column_max_index];
			m_permutations_vector_[column_max_index] = j;

			Matrix::SwapRows(m_dimensions_, *m_decompositioned_matrix_, i, column_max_index);

			++m_permutations_vector_[m_dimensions_];
		}

		for (j = i + 1; j < m_dimensions_; ++j)
		{
			m_decompositioned_matrix_->At(m_dimensions_, j, i) /= m_decompositioned_matrix_->At(m_dimensions_, i, i);

			for (k = i + 1; k < m_dimensions_; ++k)
				m_decompositioned_matrix_->At(m_dimensions_, j, k) -= m_decompositioned_matrix_->At(m_dimensions_, j, i) * m_decompositioned_matrix_->At(m_dimensions_, i, k);
		}
	}

	return true;
}

float LUDecomposition::Determinant() const
{
	if (!IsValid()) return 0;

	auto ret = m_decompositioned_matrix_->At(m_dimensions_, 0, 0);

	for (auto i = 1; i < m_dimensions_; ++i)
		ret *= m_decompositioned_matrix_->At(m_dimensions_, i, i);

	if ((m_permutations_vector_[m_dimensions_] - m_dimensions_) % 2 == 0)
		return ret;

	return -ret;
}


void LUDecomposition::Inverse(Matrix& result) const
{
	COND_ASSERT(_MATH_DEBUG_, IsValid());

	util::DynamicArray<float, 1> identity_row(m_dimensions_, 0.0f);

	for (auto i = 0; i < m_dimensions_; ++i)
	{
		if (i != 0) identity_row[i - 1] = 0;
		identity_row[i] = 1;

		auto ret_column = Solve(identity_row.Base());

		for (auto j = 0; j < m_dimensions_; ++j)
			result.At(m_dimensions_, j, i) = ret_column.At(j);
	}
}

Matrix LUDecomposition::Inverse() const
{
	Matrix ret(m_dimensions_);
	Inverse(ret);
	return ret;
}


void LUDecomposition::Solve(const float* b, Vector& result) const
{
	COND_ASSERT(_MATH_DEBUG_, IsValid());

	// solve Ly = Pb
	for (auto i = 0; i < m_dimensions_; ++i)
	{
		result.At(i) = b[m_permutations_vector_[i]];

		for (auto k = 0; k < i; ++k)
			result.At(i) -= m_decompositioned_matrix_->At(m_dimensions_, i, k) * result.At(k);
	}

	// solve Ux = y
	for (auto i = m_dimensions_ - 1; i >= 0; --i)
	{
		for (auto k = i + 1; k < m_dimensions_; ++k)
			result.At(i) -= m_decompositioned_matrix_->At(m_dimensions_, i, k) * result.At(k);

		result.At(i) /= m_decompositioned_matrix_->At(m_dimensions_, i, i);
	}
}

void LUDecomposition::Solve(const Vector& vec, Vector& result) const
{
	Solve(vec.Base(), result);
}

Vector LUDecomposition::Solve(const float* b) const
{
	Vector ret(m_dimensions_);
	Solve(b, ret);
	return ret;
}


Vector LUDecomposition::Solve(const Vector& vec) const
{
	Vector ret(m_dimensions_);
	Solve(vec, ret);
	return ret;
}


int LUDecomposition::GetRowExchangesCount() const
{
	return m_permutations_vector_[m_dimensions_];
}


bool LUDecomposition::IsValid() const
{
	return m_decompositioned_matrix_ != nullptr && m_permutations_vector_ != nullptr;
}

}

}
