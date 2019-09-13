#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

#include <NR/nr.h>

#include "linalg.h"

typedef std::chrono::system_clock::time_point timestamp_t;

struct Triangle
{
    Vector points[3];

    friend std::ostream& operator<<(std::ostream& os, const Triangle& self);
};