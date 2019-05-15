#pragma once

#include <inc/includes.h>

#define R (0)
#define G (1)
#define B (2)
#define DEPTH (3)

template<NRuint dim>
void mkTriangleInCoordinates(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, Triangle<dim>* triangle)
{
    triangle->points[0].values[0] = p0.x;
    triangle->points[0].values[1] = p0.y;

    triangle->points[1].values[0] = p1.x;
    triangle->points[1].values[1] = p1.y;
    
    triangle->points[2].values[0] = p2.x;
    triangle->points[2].values[1] = p2.y;

    for (NRuint i = 0; i < 3; ++i)
    {
        for (NRuint j = 2; j < dim; ++j)
        {
            triangle->points[i].values[j] = ((NRfloat) rand()) / RAND_MAX;
        }
    }
}

struct ColorRGB
{
    NRubyte r, g, b;

    friend bool operator==(const ColorRGB& self, const ColorRGB& other)
    {
        return self.r == other.r && self.g == other.g && self.b == other.b;
    }
};
