/**
* LU-Decomposing a matrix, while saving all relevant decomposition data.
* Done with partial pivoting (LUP decompostion) using doolitle's algorithem
*
*
* algorithm is pretty much copied from here:
*			https://en.wikipedia.org/wiki/LU_decomposition
*
*/

#ifndef MATH_ALGEBRA__LUDECOMPOSITION_H_
#define MATH_ALGEBRA__LUDECOMPOSITION_H_
#include <memory>


// -----------------------------------------------------------------------
// Forward Declerations

namespace math {

namespace algebra {

class Vector;

class Matrix;

}

}


// Forward Declerations
// -----------------------------------------------------------------------

namespace math {

namespace algebra {

class LUDecomposition
{
	std::unique_ptr<Matrix> m_decompositioned_matrix_;
	std::unique_ptr<int[]> m_permutations_vector_;
	const int m_dimensions_;

	void Cleanup();

	bool Decompose() const;

public:
	explicit LUDecomposition(int dimensions, const Matrix& mat);

	explicit LUDecomposition(int dimensions, std::unique_ptr<Matrix> mat);
	
	float Determinant() const;

	Matrix Inverse() const;

	Vector Solve(const float* b) const;

	Vector Solve(const Vector& vec) const;

	void Inverse(Matrix& result) const;

	void Solve(const float* b, Vector& result) const;

	void Solve(const Vector& vec, Vector& result) const;

	int GetRowExchangesCount() const;

	bool IsValid() const;
};

}

}

#endif // MATH_ALGEBRA__LUDECOMPOSITION_H_
