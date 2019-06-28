#include <linalg.h>
#include <cstring>
#include <cmath>

Matrix::Matrix()
{
    for (auto i = 0; i < 5; ++i)
    {
        for (auto j = 0; j < 5; ++j)
        {
            data[i][j] = 0;
        }
    }
}

Matrix::Matrix(const nr_float data[5][5])
{
    std::memcpy(this->data, data, 5 * 5 * sizeof(nr_float));
}

Matrix::Matrix(const nr_float diagonal)
{
    for (auto i = 0; i < 5; ++i)
    {
        for (auto j = 0; j < 5; ++j)
        {
            if (i == j)
                data[i][j] = diagonal;
            else
                data[i][j] = 0;
        }
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
    std::memcpy(data, other.data, 5 * 5 * sizeof(nr_float));
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) const
{
    Matrix ret;
    for (auto i = 0; i < 5; ++i)
    {
        for (auto j = 0; j < 5; ++j)
        {
            for (auto k = 0; k < 5; ++k)
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
    for (auto i = 0; i < 5; ++i)
    {
        for (auto j = 0; j < 5; ++j)
        {
            data[i][j] += other.data[i][j];
        }
    }

	return *this;
}

Vector Matrix::operator*(const Vector& other) const
{
    Vector ret;
    for (auto i = 0; i < 5; ++i)
    {
        for (auto k = 0; k < 5; ++k)
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
    Matrix ret(s);
    ret.data[4][4] = 1;
    return ret;
}

Matrix Matrix::translation(const nr_float x, const nr_float y, const nr_float z, const nr_float w)
{
    Matrix ret = Matrix::identity();
    ret.data[X][4] = x;
    ret.data[Y][4] = y;
    ret.data[Z][4] = z;
    ret.data[W][4] = w;
    return ret;
}

Matrix Matrix::identity()
{
   return Matrix(1);
}

Vector::Vector(nr_float x, nr_float y, nr_float z, nr_float w, nr_float q)
    : data{x, y, z, w, q}
{
}

Vector::Vector()
    : Vector(0, 0, 0, 0, 0)
{
}

Vector::Vector(const Vector4d& other)
    : Vector(other.x, other.y, other.z, other.w, 1)
{
}

Vector4d Vector::toVector4d() const
{
    Vector4d ret;
    toVector4d(ret);
    return ret;
}

void Vector::toVector4d(Vector4d& vec) const
{
    vec.x = data[X];
    vec.y = data[Y];
    vec.z = data[Z];
    vec.w = data[W];
}

void Vector::toVector4d(Vector4d* vec) const
{
    vec->x = data[X];
    vec->y = data[Y];
    vec->z = data[Z];
    vec->w = data[W];
}

nr_float& Vector::operator[](const Axis idx)
{
    return data[idx];
}

const nr_float& Vector::operator[](const Axis idx) const
{
    return data[idx];
}

std::ostream& operator<<(std::ostream& os, const Vector& self)
{
    return os << "Vector4d{" << self[X] << ", " << self[Y] << ", " << self[Z] << ", " << self[W] << ", " << self[Q] << "}";
}

