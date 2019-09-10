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
    const float3 near, const float3 far,
    global point_t* result)
{
    const uint index = get_global_id(0);
     
	result[index].w = points[index].w;
	result[index].z = normalize_range(points[index].z, near.z, far.z);
    result[index].y = 2 * (near.z * points[index].y / points[index].z - near.y) / (far.y - near.y) - 1;
    result[index].x = 2 * (near.z * points[index].x / points[index].z - near.x) / (far.x - near.x) - 1;
	REPORT_GLOBAL2("[%v4f] -> [%v4f]\n", points[index], result[index]);
}

)__CODE__";

}

}

}