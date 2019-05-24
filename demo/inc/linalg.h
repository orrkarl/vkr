#pragma once

#include <ostream>

#include "utils.h"

#define X (0)
#define Y (1)
#define Z (2)
#define W (3)
#define Q (4)

struct Vector
{
    NRfloat data[5];
    
    explicit Vector(NRfloat x, NRfloat y, NRfloat z, NRfloat w, NRfloat q);
    explicit Vector();
    explicit Vector(const Vector4d& other);

    Vector4d toVector4d() const;
    void toVector4d(Vector4d& vec) const;
    void toVector4d(Vector4d* vec) const;

    NRfloat& operator[](const NRuint idx);
    const NRfloat& operator[](const NRuint idx) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec);
};

struct Matrix
{
    NRfloat data[5][5];

    Matrix();
    Matrix(const NRfloat data[5][5]);
    Matrix(const NRfloat diagonal);
    Matrix(const Matrix& other);
    Matrix(const Matrix&& other);

    Matrix& operator=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;
    Matrix& operator*=(const Matrix& other);

    Matrix operator+(const Matrix& other) const;
    Matrix& operator+=(const Matrix& other);

    Vector operator*(const Vector& vec) const;

    static Matrix rotation(const NRuint axis0, const NRuint axis1, const NRfloat radians);
    static Matrix scale(const NRfloat s);
    static Matrix translation(const NRfloat x, const NRfloat y, const NRfloat z, const NRfloat w);
    static Matrix identity();
};
