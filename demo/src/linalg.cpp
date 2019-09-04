#include "linalg.h"

#include <cstring>
#include <cmath>

Matrix::Matrix()
	: data{}
{
}

Matrix::Matrix(const nr_float data[4][4])
	: data{}
{
    std::memcpy(this->data, data, 4 * 4 * sizeof(nr_float));
}

Matrix::Matrix(const nr_float diagonal)
	: data{}
{
    for (auto i = 0; i < 4; ++i)
    {
		data[i][i] = diagonal;
    }
}

Matrix::Matrix(const Matrix& other)
    : Matrix(other.data)
{
}

Matrix::Matrix(Matrix&& other)
    : Matrix(other.data)
{
}

Matrix& Matrix::operator=(const Matrix& other)
{
    std::memcpy(data, other.data, 4 * 4 * sizeof(nr_float));
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    Matrix ret;
    for (auto i = 0; i < 4; ++i)
    {
        for (auto j = 0; j < 4; ++j)
        {
            for (auto k = 0; k < 4; ++k)
            {
                ret.data[i][j] += data[i][k] * other.data[k][j];
            }
        }
    }
    return ret;
}

Matrix& Matrix::operator*=(const Matrix& other)
{
    Matrix tmp = *this;
    return (*this = tmp * other);
}

Matrix Matrix::operator+(const Matrix& other) const
{
    Matrix ret = *this;
    return ret += other;
}

Matrix& Matrix::operator+=(const Matrix& other)
{
    for (auto i = 0; i < 4; ++i)
    {
        for (auto j = 0; j < 4; ++j)
        {
            data[i][j] += other.data[i][j];
        }
    }

	return *this;
}

Vector Matrix::operator*(const Vector& other) const
{
    Vector ret;
    for (auto i = 0; i < 4; ++i)
    {
        for (auto k = 0; k < 4; ++k)
        {
            ret.data[i] += data[i][k] * other.data[k];
        }
    }
    return ret;
}

Matrix Matrix::rotation(const Axis source, const Axis dest, const nr_float radians)
{
    Matrix ret = Matrix::identity();

    nr_float cosine = cos(radians);
    nr_float sine   = sin(radians);

    ret.data[source][source] = cosine;
    ret.data[source][dest] = -sine;
    ret.data[dest][source] = sine;
    ret.data[dest][dest] = cosine;

    return ret;
}

Matrix Matrix::scale(const nr_float s)
{
	return Matrix::scale(s, s, s);
}

Matrix Matrix::scale(const nr_float x, const nr_float y, const nr_float z)
{
	Matrix ret(1.0f);
	ret.data[0][0] = x;
	ret.data[1][1] = y;
	ret.data[2][2] = z;
	return ret;
}

Matrix Matrix::translation(const nr_float x, const nr_float y, const nr_float z)
{
    Matrix ret = Matrix::identity();
    ret.data[X][3] = x;
    ret.data[Y][3] = y;
    ret.data[Z][3] = z;
    return ret;
}

Matrix Matrix::identity()
{
   return Matrix(1);
}

std::ostream& operator<<(std::ostream& os, const Matrix& mat)
{
	for (auto i = 0u; i < 4; ++i)
	{
		os << "[ ";
		for (auto j = 0u; j < 4; ++j)
		{
			os << mat.data[i][j] << " ";
		}
		os << "]\n";
	}

	return os;
}

Vector::Vector(nr_float x, nr_float y, nr_float z, nr_float w)
    : data{x, y, z, w}
{
}

Vector::Vector(nr_float x, nr_float y, nr_float z)
	: Vector(x, y, z, 1.0f)
{
}

Vector::Vector()
    : Vector(0, 0, 0, 0)
{
}

nr_float& Vector::operator[](const Axis idx)
{
    return data[idx];
}

const nr_float& Vector::operator[](const Axis idx) const
{
    return data[idx];
}

nr_float Vector::dot(const Vector& other) const
{
	return data[0] * other.data[0] + data[1] * other.data[1] + data[2] * other.data[2] + data[3] * other.data[3];
}

nr_float Vector::distanceSquared(const Vector& other) const
{
	auto tmp = *this - other;
	return tmp.dot(tmp);
}

Vector Vector::operator-(const Vector& other) const
{
	Vector ret;
	ret.data[0] = data[0] - other.data[0];
	ret.data[1] = data[1] - other.data[1];
	ret.data[2] = data[2] - other.data[2];
	ret.data[3] = data[3] - other.data[3];
	return ret;
}

Vector Vector::operator+(const Vector& other) const
{
	Vector ret;
	ret.data[0] = data[0] + other.data[0];
	ret.data[1] = data[1] + other.data[1];
	ret.data[2] = data[2] + other.data[2];
	ret.data[3] = data[3] + other.data[3];
	return ret;
}

Vector Vector::operator*(const nr_float s) const
{
	Vector ret;
	ret.data[0] = data[0] * s;
	ret.data[1] = data[1] * s;
	ret.data[2] = data[2] * s;
	ret.data[3] = data[3] * s;
	return ret;
}

Vector Vector::operator/(const nr_float s) const
{
	Vector ret;
	ret.data[0] = data[0] / s;
	ret.data[1] = data[1] / s;
	ret.data[2] = data[2] / s;
	ret.data[3] = data[3] / s;
	return ret;
}

bool Vector::operator==(const Vector& other) const
{
	return data[0] == other.data[0] && data[1] == other.data[1] && data[2] == other.data[2] && data[3] == other.data[3];
}

std::ostream& operator<<(std::ostream& os, const Vector& self)
{
    return os << "Vector4d{" << self[X] << ", " << self[Y] << ", " << self[Z] << ", " << self[W] << ", " << "}";
}

