 #include "vertex_reducer.cl.h"

namespace nr
{

namespace detail
{

extern const char* VERTEX_REDUCE_KERNEL_NAME = "shade_vertex";

namespace clcode
{

extern const char* vertex_reduce = R"__CODE__(

// Normalizes value to [0,1], when value is expected to be in [near, far]
float normalize_range(const float value, const float near, const float far)
{
    return (value - near) / (far - near);
}

// Applies the perspective projection (with normalization) to a specific coordinate
float perspective_bounded_axis(
    const float axis_value,
    const float projection_value,
    const float min, const float max)
{
	return 2 * normalize_range(axis_value / projection_value, min, max) - 1;
}

// vertex shader "main" function. Applies all of the perspective steps to a vector.
kernel void shade_vertex(
    const global point_t* points, 
    const global float near[3], const global float far[3],
    global point_t* result)
{
    const uint index = get_global_id(0);
     
	result[index].w = points[index].w;
	result[index].z = normalize_range(points[index].z, near[2], far[2]);
    result[index].y = perspective_bounded_axis(points[index].y, points[index].z, near[1], far[1]);
    result[index].x = perspective_bounded_axis(points[index].x, points[index].z, near[0], far[0]);
}

)__CODE__";

}

}

}