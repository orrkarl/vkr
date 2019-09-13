 #include "vertex_reducer.cl.h"

namespace nr
{

namespace detail
{

extern const char* VERTEX_REDUCE_KERNEL_NAME = "shade_vertex";

namespace clcode
{

extern const char* vertex_reduce = R"__CODE__(

// vertex shader "main" function. Applies all of the perspective steps to a vector.
kernel void shade_vertex(
    const global point_t* points, 
    const float aspect_ratio,
	const float scale,
	const float near, const float far,
    global point_t* result)
{
    const uint index = get_global_id(0);
     
	result[index].w = points[index].w;
	result[index].z = (points[index].z - near) / (far - near);
    result[index].y = scale * 2 * near * points[index].y / points[index].z;
    result[index].x = aspect_ratio * scale * 2 * near * points[index].x / points[index].z;
}

)__CODE__";

}

}

}