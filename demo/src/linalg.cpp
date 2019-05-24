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

Matrix::Matrix(const NRfloat data[5][5])
{
    std::memcpy(this->data, data, 5 * 5 * sizeof(NRfloat));
}

Matrix::Matrix(const NRfloat diagonal)
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

Matrix::Matrix(const Matrix&& other)
    : Matrix(other.data)
{
}

Matrix& Matrix::operator=(const Matrix& other)
{
    std::memcpy(data, other.data, 5 * 5 * sizeof(NRfloat));
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

Matrix Matrix::rotation(const NRuint axis0, const NRuint axis1, const NRfloat radians)
{
    Matrix ret = Matrix::identity();

    NRfloat cosine = cos(radians);
    NRfloat sine   = sin(radians);

    ret.data[axis0][axis0] = cosine;
    ret.data[axis0][axis1] = -sine;
    ret.data[axis1][axis0] = sine;
    ret.data[axis1][axis1] = cosine;

    return ret;
}

Matrix Matrix::scale(const NRfloat s)
{
    Matrix ret(s);
    ret.data[4][4] = 1;
    return ret;
}

Matrix Matrix::translation(const NRfloat x, const NRfloat y, const NRfloat z, const NRfloat w)
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

Vector::Vector(NRfloat x, NRfloat y, NRfloat z, NRfloat w, NRfloat q)
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

NRfloat& Vector::operator[](const NRuint idx)
{
    return data[idx];
}

const NRfloat& Vector::operator[](const NRuint idx) const
{
    return data[idx];
}

std::ostream& operator<<(std::ostream& os, const Vector& self)
{
    return os << "Vector4d{" << self[X] << ", " << self[Y] << ", " << self[Z] << ", " << self[W] << ", " << self[Q] << "}";
}

