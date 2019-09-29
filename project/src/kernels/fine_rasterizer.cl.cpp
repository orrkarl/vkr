#include "fine_rasterizer.cl.h"

namespace nr
{

namespace detail
{

extern const char* FINE_RASTER_KERNEL_NAME = "fine_rasterize";

namespace clcode
{

extern const char* fine_rasterizer = R"__CODE__(

// Apply a fragment to the framebuffer (if it passes the depth test)
void shade(
    fragment_t fragment,
    const screen_dimension_t dim,
    global raw_color_rgba_t* color, global depth_t* depth)
{
    uint buffer_index;
    buffer_index = index_from_screen(fragment.position, dim);
        
    if (fragment.depth >= depth[buffer_index])
    {
        fragment.color = RAW_RED; // replace this when you get to the actual shading scheme
    
        color[buffer_index] = fragment.color;
        depth[buffer_index] = fragment.depth;
    }
}

// Calculate the signed area of a parallelogram
float area(const ndc_position_t p0, const ndc_position_t p1, const ndc_position_t p2)
{
    float2 a = (float2)(p1.x - p0.x, p1.y - p0.y);
    float2 b = (float2)(p0.x - p2.x, p0.y - p2.y);

    return a.x * b.y - a.y * b.x;
}

// Calculate 2d barycentric coordinates
void barycentric2d(const ndc_position_t p0, const ndc_position_t p1, const ndc_position_t p2, ndc_position_t position, float3* result)
{
    float area_total = area(p0, p1, p2);

	*result = (float3)(area(position, p1, p2) / area_total, area(p0, position, p2) / area_total, area(p0, p1, position) / area_total);
}

// Calculate (according to Perspective Correct Interpolation) the inverse of the depth at point
depth_t depth_at_point(const triangle_record_t record, float3 barycentric)
{
    return barycentric.x / record.triangle.p0.z + barycentric.y / record.triangle.p1.z + barycentric.z / record.triangle.p2.z;
}


// Check if point fits the top\left rule for a certain edge
bool is_contained_top_left(const float2 vec, float weight)
{
	return weight > 0 || (weight == 0 && (vec.y > 0 || (vec.y == 0 && vec.x > 0)));
}

// Check if a point is "in" a triangle, according to the top\left rule
bool is_point_in_triangle(const ndc_position_t p0, const ndc_position_t p1, const ndc_position_t p2, float3 barycentric)
{		
	float2 v0 = (float2)(p2.x - p1.x, p2.y - p1.y);
    float2 v1 = (float2)(p0.x - p2.x, p0.y - p2.y);
    float2 v2 = (float2)(p1.x - p0.x, p1.y - p0.y);

    return is_contained_top_left(v0, barycentric.x) && is_contained_top_left(v1, barycentric.y) && is_contained_top_left(v2, barycentric.z);
}

bool is_queue_ended(global const index_t* triangle_queue, uint queue_head_idx, uint queue_size)
{
    return queue_head_idx >= queue_size || (!triangle_queue[queue_head_idx] && queue_head_idx);
}

// Pick the next non-empty bin queue
uint pick_queue(global const index_t** triangle_queues, uint* current_queue_heads, const uint work_group_count, const uint queue_size)
{
    uint current_queue = work_group_count;

    for (uint i = 0; i < work_group_count; ++i)
    {
        if (!is_queue_ended(triangle_queues[i], current_queue_heads[i], queue_size))
        {
            if (current_queue == work_group_count)
            {
                current_queue = i;
            }
            else if (triangle_queues[i][current_queue_heads[i]] < triangle_queues[current_queue][current_queue_heads[current_queue]])
            {
                current_queue = i;
            }
        }
    }

    return current_queue;
}

kernel void fine_rasterize(
    const global triangle_record_t* triangle_data,
    const global index_t* bin_queues,
    const screen_dimension_t screen_dim,
    const bin_queue_config_t config,
    const uint work_group_count,
    global raw_color_rgba_t* color,
    global depth_t* depth)
{
    fragment_t current_frag;
    float3 barycentric;
    uint current_queue_elements[MAX_WORK_GROUP_COUNT];
    global const index_t* current_queue_bases[MAX_WORK_GROUP_COUNT];

    private ndc_position_t current_position_ndc;
    
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint bin_count_x = ceil(((float) screen_dim.width) / config.bin_width);
    const uint bin_count_y = ceil(((float) screen_dim.height) / config.bin_height);

    const uint bin_queue_offset = (config.queue_size + 1) * (y * bin_count_x + x);
    const uint elements_per_group = (config.queue_size + 1) * bin_count_x * bin_count_y; 

    uint current_queue;
    index_t current_queue_element;
    
    ndc_position_t p0, p1, p2;

    if (work_group_count >= MAX_WORK_GROUP_COUNT)
    {
        return;
    }
    
    for (uint i = 0; i < work_group_count; ++i)
    {
        current_queue_bases[i] = bin_queues + bin_queue_offset + i * elements_per_group;
		
        if (current_queue_bases[i][0]) 
        {
            current_queue_elements[i] = config.queue_size + 1;
        }
        else
        {
            current_queue_elements[i] = 0;
        }

        current_queue_bases[i] += 1;
    }

    while (true)
    {
        current_queue = pick_queue(current_queue_bases, current_queue_elements, work_group_count, config.queue_size);
        
        if (current_queue >= work_group_count) 
        {
            break;
        }
        
        current_queue_element = current_queue_bases[current_queue][current_queue_elements[current_queue]];

		p0.x = triangle_data[current_queue_element].triangle.p0.x;
		p0.y = triangle_data[current_queue_element].triangle.p0.y;

		p1.x = triangle_data[current_queue_element].triangle.p1.x;
		p1.y = triangle_data[current_queue_element].triangle.p1.y;

		p2.x = triangle_data[current_queue_element].triangle.p2.x;
		p2.y = triangle_data[current_queue_element].triangle.p2.y;

        for (uint frag_x = x * config.bin_width; frag_x < min(screen_dim.width, x * config.bin_width + config.bin_width); ++frag_x)
        {
            for (uint frag_y = y * config.bin_height; frag_y < min(screen_dim.height, y * config.bin_height + config.bin_height); ++frag_y)
            {
                current_frag.position.x = frag_x;
                current_frag.position.y = frag_y;
                
                pixel_mid_point_from_screen(current_frag.position, screen_dim, &current_position_ndc); 

                barycentric2d(p0, p1, p2, current_position_ndc, &barycentric);

                if (is_point_in_triangle(p0, p1, p2, barycentric))
                {
                    current_frag.depth = depth_at_point(triangle_data[current_queue_element], barycentric);
                    shade(current_frag, screen_dim, color, depth);
                }
            }
        }

        current_queue_elements[current_queue] += 1;
    }
}

// -------------------------------------- Tests --------------------------------------

#ifdef _TEST_FINE

kernel void shade_test(
    const fragment_t fragment,
    const screen_dimension_t dim,
    global raw_color_rgba_t* color, global depth_t* depth)
{
    shade(fragment, dim, color, depth);
}

kernel void is_point_in_triangle_test(const triangle_record_t record, const screen_position_t position, const screen_dimension_t dim, global bool* result)
{
    ndc_position_t pos;
    
    ndc_position_t p0, p1, p2;
    
	p0.x = record.triangle.p0.x;
	p0.y = record.triangle.p0.y;

    p1.x = record.triangle.p1.x;
	p1.y = record.triangle.p1.y;

    p2.x = record.triangle.p2.x;
	p2.y = record.triangle.p2.y;
    
    ndc_from_screen_p(position, dim, &pos);

    float3 barycentric;
    barycentric2d(p0, p1, p2, pos, &barycentric);
    *result = is_point_in_triangle(p0, p1, p2, barycentric);
}

#endif // _TEST_FINE

)__CODE__";

}

}

}