#include "vertex_post_processor.cl.h"

namespace nr
{

namespace detail
{

extern const char* TRIANGLE_SETUP_KERNEL_NAME = "triangle_setup";

namespace clcode
{

extern const char* vertex_post_processor = R"__CODE__(

#define TRIANGLE_EPSILON (1e-5)

typedef struct _triangle_record
{
	triangle_t triangle;
} triangle_record_t;

float area(const triangle_t triangle)
{
	float2 vec0 = triangle.p1.xy - triangle.p0.xy;
	float2 vec1 = triangle.p2.xy - triangle.p1.xy;
	return vec0.x * vec1.y - vec1.x * vec0.y;
}

bool ccw(const float area)
{
	return area > TRIANGLE_EPSILON;
}

bool degenerate(const float area)
{
	return fabs(area) <= TRIANGLE_EPSILON;
}

bool cw(const float area)
{
	return area < -TRIANGLE_EPSILON;
}

kernel void vertex_post_processing(
	const global triangle_t* triangles,
	global triangle_record_t* records)
{
	private const index_t index = get_global_id(0);
	private const triangle_t triangle = triangles[index];

	float triangle_area = area(triangle);

	if (ccw(triangle_area) || degenerate(triangle_area))
	{
		records[index].triangle.p0.x = NAN;
		return;
	}

	records[index].triangle = triangle;
}


// -------------------------------------- Testing --------------------------------------

kernel void test_ccw(const triangle_t triangle, global uint* is_ccw)
{
	*is_ccw = ccw(area(triangle));
}

kernel void test_cw(const triangle_t triangle, global uint* is_cw)
{
	*is_cw = cw(area(triangle));
}

kernel void test_degenerate(const triangle_t triangle, global uint* is_degenerate)
{
	*is_degenerate = degenerate(area(triangle));
}

kernel void test_area(const triangle_t triangle, global float* res_area)
{
	*res_area = area(triangle);
}

)__CODE__";

}

}

}