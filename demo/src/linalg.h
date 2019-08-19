#pragma once

#include <ostream>

#include <NR/nr.h>

typedef enum _Axis
{
	X = 0, Y = 1, Z = 2, W = 3, Q = 4
} Axis;

struct Vector
{
    nr_float data[5];
    
    explicit Vector(nr_float x, nr_float y, nr_float z, nr_float w, nr_float q);
    explicit Vector();

    nr_float& operator[](const Axis axis);
    const nr_float& operator[](const Axis axis) const;

	nr_float dot(const Vector& other) const;
	nr_float distanceSquared(const Vector& other) const;

	Vector operator-(const Vector& other) const;
	Vector operator+(const Vector& other) const;
	Vector operator*(const nr_float s) const;
	Vector operator/(const nr_float s) const;

	bool operator==(const Vector& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec);
};

struct Matrix
{
    nr_float data[5][5];

    Matrix();
    Matrix(const nr_float data[5][5]);
    Matrix(const nr_float diagonal);
    Matrix(const Matrix& other);
    Matrix(Matrix&& other);

    Matrix& operator=(const Matrix& other);

    Matrix operator*(const Matrix& other) const;
    Matrix& operator*=(const Matrix& other);

    Matrix operator+(const Matrix& other) const;
    Matrix& operator+=(const Matrix& other);

    Vector operator*(const Vector& vec) const;

    static Matrix rotation(const Axis source, const Axis dest, const nr_float radians);
    static Matrix scale(const nr_float s);
	static Matrix scale(const nr_float x, const nr_float y, const nr_float z, const nr_float w);
    static Matrix translation(const nr_float x, const nr_float y, const nr_float z, const nr_float w);
    static Matrix identity();

	friend std::ostream& operator<<(std::ostream& os, const Matrix& mat);
};
