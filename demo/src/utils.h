#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

#include <NR/nr.h>

#include "linalg.h"

typedef std::chrono::system_clock::time_point timestamp_t;

struct Triangle4d
{
    Vector points[3];

    friend std::ostream& operator<<(std::ostream& os, const Triangle4d& self);
};

struct Tetrahedron
{
    Vector points[4];

    friend std::ostream& operator<<(std::ostream& os, const Tetrahedron& self);
};

// Reduce a 4-Simplex to Triangles
void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4]);

void cube4dToSimplices(const Vector cube[16], Tetrahedron simplices[6 * 8]);
