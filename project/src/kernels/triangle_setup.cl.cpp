#include "triangle_setup.cl.h"

namespace nr
{

namespace detail
{

extern const char* TRIANGLE_SETUP_KERNEL_NAME = "triangle_setup";

namespace clcode
{

extern const char* triangle_setup = R"__CODE__(

#define TRIANGLE_EPSILON (1e-5)

typedef struct _triangle_record
{
	triangle_t	main_triangle;
	index_t		sub_triangles_index;
	index_t		triangle_count;
} triangle_record_t;

typedef struct _triangle_allocation_buffer
{
	global index_t*	current_buffer_head;
	const index_t	max_triangle_count;
} triangle_allocation_buffer_t;

index_t malloc(triangle_allocation_buffer_t allocator, const index_t count)
{
	index_t res = atomic_add(allcoator.current_buffer_head, count);
	return select(res, INVALID_INDEX, res >= allocator.max_triangle_count);
}

float area(const triangle_t triangle)
{
	float2 vec0 = triangle.s1.xy - triangle.s0.xy;
	float2 vec1 = triangle.s2.xy - triangle.s1.xy;
	return vec0.x * vec1.y - vec1.x * vec0.y;
}

bool ccw(const float area)
{
	return area > TRIANGLE_EPSILON;
}

bool degenerate(const float area)
{
	return abs(area) <= TRIANGLE_EPSILON;
}

bool cw(const float area)
{
	return area < TRIANGLE_EPSILON;
}

index_t count_clipping_subtris(const triangle_t base)
{
	return 0;
}

void clip(const triangle_t base, global triangle_t* main, global triangle_t* subtris)
{

}

kernel void triangle_setup(
	const global triangle_t*  triangles,
	global triangle_t*		  dynamic_allocation_pool,
	global index_t*			  current_buffer_head,
	const index_t			  max_triangle_count,
	global triangle_record_t* records)
{
	private const index_t index = get_global_id(0);
	private const triangle_t triangle = triangles[index];
	
	private triangle_record_t* dest = records + index;

	float triangle_area = area(triangle);

	if (ccw(triangle_area) || degenerate(triangle_area))
	{
		dest->triangle_count = 0;
		return;
	}

	subtris_count = count_clipping_subtris(triangle);
	dest->triangle_count = subtris_count + 1;

	if (subtris_count)
	{
		subtris_index = malloc({ current_buffer_head, max_triangle_count}, subtris_count);
		if (dest->subtris_index == INVALID_INDEX)
		{
			return;
		}
		dest->sub_triangles_index = subtris_index;
		clip(triangle, &(dest->main), &(dynamic_allocation_pool[subtris_index]));
	}
	else
	{
		records[index].sub_triangles_index = INVALID_INDEX;
		records[index].main_triangle = triangle;
	}
}

)__CODE__";

}

}

}