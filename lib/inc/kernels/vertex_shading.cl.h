#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string vertex_shading = R"__CODE__(

// Normalizes value to [0,1], when value is expected to be in [near, far]
float normalize_range(const float value, const float near, const float far)
{
    return (value - near) / (far - near);
}

// Normalizes a given coordinate to between the relevant vnear/far values
void normalize_step(
    const global Point p, 
    const global float near[RENDER_DIMENSION], const global float far[RENDER_DIMENSION],
    const uint d, 
    global Point result)
{
    result[d] = normalize_range(p[d], near[d], far[d]);
}

// Applies the perspective projection (without normalization) to a vector
void perspective_step(const uint d, global Point result)
{ 
    for (uint coord = 0; coord < d; ++coord)
    {
        result[coord] /= result[d];
    }
}

// Applies the perspective projection (with normalization) to a specific coordinate
void perspective_bounded_axis(
    const float axis_value,
    const float projection_value,
    const float min, const float max,
    global float* result)
{
    *result = (2 * axis_value / projection_value - max - min) / (max - min);
}

// Applies the perspective projection (with normalization) to a vector
void perspective_bounded(
    const float left, const float right, const float bottom, const float top,
    global Point result)
{
    global float* p_floats = (global float*) result;
    perspective_bounded_axis(result[0], result[2], left, right, p_floats);
    perspective_bounded_axis(result[1], result[2], bottom, top, p_floats + 1);
}


// vertex shader "main" function. Applies all of the perspective steps to a vector.
kernel void shade_vertex(
    const global Point* points, 
    const global float near[RENDER_DIMENSION], const global float far[RENDER_DIMENSION],
    global Point* result)
{
    const uint index = get_global_id(0);
        
    for (uint d = 0; d < ELEMENTS_PER_POINT; ++d)
    {
        result[index][d] = points[index][d];
    }
    
    for (uint d = RENDER_DIMENSION - 1; d > 2; --d)
    {
        perspective_step(d, result[index]); 
        normalize_step(points[index], near, far, d, result[index]);
    }

    perspective_bounded(near[0], far[0], near[1], far[1], result[index]);
    normalize_step(points[index], near, far, 2, result[index]);
}

)__CODE__";

}

}

}