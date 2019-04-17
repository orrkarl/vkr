#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <general/predefs.h>

#pragma once

const float TOLERANCE = 0.00001f;
const double DOUBLE_TOLERANCE = 0.000000001;

template<NRuint Dim>
struct Point
{
    NRfloat values[Dim];
};

template<NRuint Dim>
struct Simplex
{
    Point<Dim> points[Dim];
};
