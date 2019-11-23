#include "vertex_post_processor.cl.h"

namespace nr
{

namespace detail
{

extern const char* TRIANGLE_SETUP_KERNEL_NAME = "vertex_post_processing";

namespace clcode
{

extern const char* vertex_post_processor = R"__CODE__(

#define TRIANGLE_EPSILON (1e-7)

bool valid(const triangle_t triangle)
{
	return all(isfinite(triangle.p0)) && all(isfinite(triangle.p1)) && all(isfinite(triangle.p2));
}

bool depth_positive(const triangle_t triangle)
{
	return triangle.p0.z > 0 && triangle.p1.z > 0 && triangle.p2.z > 0;
}

float parallelogram_area(const triangle_t triangle)
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

void discard(global triangle_record_t* record)
{
	record->triangle.p0.x = NAN;
}

kernel void vertex_post_processing(
	const global triangle_t* triangles,
	global triangle_record_t* records)
{
	private const index_t index = get_global_id(0);
	private const triangle_t triangle = triangles[index];

	if (!valid(triangle) || !depth_positive(triangle))
	{
		discard(&records[index]);
		return;
	}

	float area = parallelogram_area(triangle);

	if (!cw(area))
	{
		discard(&records[index]);
		return;
	}
	
	records[index].triangle = triangle;
}


// -------------------------------------- Testing --------------------------------------

#ifdef _DEBUG

kernel void test_area(const triangle_t triangle, global float* res_area)
{
	*res_area = 0.5 * parallelogram_area(triangle);
}

#endif

)__CODE__";

}

}

}