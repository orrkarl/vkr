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

kernel void vertex_post_processing(
	const global triangle_t* triangles,
	global triangle_record_t* records)
{
	private const index_t index = get_global_id(0);
	private const triangle_t triangle = triangles[index];

	if (!valid(triangle))
	{
		records[index].triangle.p0.x = NAN;
		return;
	}

	float area = parallelogram_area(triangle);
	//REPORT_GLOBAL1("area: %f", 0.5 * area);

	if (ccw(area) || degenerate(area))
	{
		records[index].triangle.p0.x = NAN;
		//REPORT_GLOBAL("culluing triangle!");
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