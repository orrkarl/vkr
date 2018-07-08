#include "lu_decomposition.h"

#include "matrix.h"
#include "vector.h"


namespace math {
	
namespace algebra {

LUDecomposition::LUDecomposition(const Matrix& mat) 
	: m_decompositioned_matrix_(std::make_unique<Matrix>(mat.GetDimensions())), m_permutations_vector_(new int[mat.GetDimensions() + 1])
{
	const auto size = mat.GetDimensions();

	for (auto i = 0; i <= size; ++i)
		m_permutations_vector_[i] = i;

	for (auto i = 0; i < size; ++i)
		for (auto j = 0; j < size; ++j)
			m_decompositioned_matrix_->At(i, j) = mat(i, j);

	if (!Decompose())
	{
		delete[] m_permutations_vector_;
		m_decompositioned_matrix_.reset();
		m_decompositioned_matrix_ = nullptr;
		m_permutations_vector_ = nullptr;
	}
}

LUDecomposition::LUDecomposition(const std::unique_ptr<Matrix> mat)
	: m_decompositioned_matrix_(std::make_unique<Matrix>(mat->GetDimensions())), m_permutations_vector_(new int[mat->GetDimensions() + 1])
{
	for (auto i = 0; i <= mat->GetDimensions(); ++i)
		m_permutations_vector_[i] = i;

	if (!Decompose())
	{
		delete[] m_permutations_vector_;
		m_decompositioned_matrix_.reset();
		m_decompositioned_matrix_ = nullptr;
		m_permutations_vector_ = nullptr;
	}
}

LUDecomposition::~LUDecomposition()
{
	delete[] m_permutations_vector_;
}


bool LUDecomposition::Decompose() const
{
	int j, k;
	float current_absolute_value;

	for (auto i = 0; i < m_decompositioned_matrix_->GetDimensions(); ++i)
	{
		float column_max = 0;
		auto column_max_index = i;

		for (k = i; k < m_decompositioned_matrix_->GetDimensions(); ++k)
		{
			if ((current_absolute_value = abs(m_decompositioned_matrix_->At(k, i))) > column_max)
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

			Matrix::SwapRows(*m_decompositioned_matrix_, i, column_max_index);

			++m_permutations_vector_[m_decompositioned_matrix_->GetDimensions()];
		}

		for (j = i + 1; j < m_decompositioned_matrix_->GetDimensions(); ++j)
		{
			m_decompositioned_matrix_->At(j, i) /= m_decompositioned_matrix_->At(i, i);

			for (k = i + 1; k < m_decompositioned_matrix_->GetDimensions(); ++k)
				m_decompositioned_matrix_->At(j, k) -= m_decompositioned_matrix_->At(j, i) * m_decompositioned_matrix_->At(i, k);
		}
	}

	return true;
}

LUDecomposition LUDecomposition::MakeDecomposition(const Matrix& mat)
{
	LUDecomposition decomp(mat);

	if (decomp.IsValid())
		return decomp;
	
	return static_cast<LUDecomposition>(nullptr);
}

float LUDecomposition::Determinant() const
{
	if (!IsValid()) return 0;

	auto ret = m_decompositioned_matrix_->At(0, 0);

	for (auto i = 1; i < m_decompositioned_matrix_->GetDimensions(); ++i)
		ret *= m_decompositioned_matrix_->At(i, i);

	if ((m_permutations_vector_[m_decompositioned_matrix_->GetDimensions()] - m_decompositioned_matrix_->GetDimensions()) % 2 == 0)
		return ret;

	return -ret;
}


void LUDecomposition::Inverse(Matrix& result) const
{
	util::DynamicArray<float, 1> identity_row(m_decompositioned_matrix_->GetDimensions(), true);

	for (auto i = 0; i < m_decompositioned_matrix_->GetDimensions(); ++i)
	{
		if (i != 0) identity_row[i - 1] = 0;
		identity_row[i] = 1;

		auto ret_column = Solve(identity_row.Base());

		for (auto j = 0; j < m_decompositioned_matrix_->GetDimensions(); ++j)
			result(j, i) = ret_column(j);
	}
}

Matrix LUDecomposition::Inverse() const
{
	Matrix ret(m_decompositioned_matrix_->GetDimensions());
	Inverse(ret);
	return ret;
}


void LUDecomposition::Solve(const float* b, Vector& result) const
{
	ASSERT(IsValid());

	// solve Ly = Pb
	for (auto i = 0; i < m_decompositioned_matrix_->GetDimensions(); ++i)
	{
		result(i) = b[m_permutations_vector_[i]];

		for (auto k = 0; k < i; ++k)
			result(i) -= m_decompositioned_matrix_->At(i, k) * result(k);
	}

	// solve Ux = y
	for (auto i = m_decompositioned_matrix_->GetDimensions() - 1; i >= 0; --i)
	{
		for (auto k = i + 1; k < m_decompositioned_matrix_->GetDimensions(); ++k)
			result(i) -= m_decompositioned_matrix_->At(i, k) * result(k);

		result(i) /= m_decompositioned_matrix_->At(i, i);
	}
}

void LUDecomposition::Solve(const Vector& vec, Vector& result) const
{
	Solve(vec.Base(), result);
}

Vector LUDecomposition::Solve(const float* b) const
{
	Vector ret(m_decompositioned_matrix_->GetDimensions());
	Solve(b, ret);
	return ret;
}


Vector LUDecomposition::Solve(const Vector& vec) const
{
	Vector ret(m_decompositioned_matrix_->GetDimensions());
	Solve(vec, ret);
	return ret;
}


int LUDecomposition::GetRowExchangesCount() const
{
	return m_permutations_vector_[m_decompositioned_matrix_->GetDimensions()];
}


bool LUDecomposition::IsValid() const
{
	return m_decompositioned_matrix_ != nullptr && m_permutations_vector_ != nullptr;
}

}

}
