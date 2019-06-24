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
    nr_float data[5];
    
    explicit Vector(nr_float x, nr_float y, nr_float z, nr_float w, nr_float q);
    explicit Vector();
    explicit Vector(const Vector4d& other);

    Vector4d toVector4d() const;
    void toVector4d(Vector4d& vec) const;
    void toVector4d(Vector4d* vec) const;

    nr_float& operator[](const nr_uint idx);
    const nr_float& operator[](const nr_uint idx) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec);
};

struct Matrix
{
    nr_float data[5][5];

    Matrix();
    Matrix(const nr_float data[5][5]);
    Matrix(const nr_float diagonal);
    Matrix(const Matrix& other);
    Matrix(const Matrix&& other);

    Matrix& operator=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;
    Matrix& operator*=(const Matrix& other);

    Matrix operator+(const Matrix& other) const;
    Matrix& operator+=(const Matrix& other);

    Vector operator*(const Vector& vec) const;

    static Matrix rotation(const nr_uint axis0, const nr_uint axis1, const nr_float radians);
    static Matrix scale(const nr_float s);
    static Matrix translation(const nr_float x, const nr_float y, const nr_float z, const nr_float w);
    static Matrix identity();
};
