#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(

// Apply a fragment to the framebuffer (if it passes the depth test)
void shade(
    Fragment fragment,
    const ScreenDimension dim,
    global RawColorRGBA* color, global Depth* depth)
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
float area(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2)
{
    float2 a = (float2)(p1.x - p0.x, p1.y - p0.y);
    float2 b = (float2)(p0.x - p2.x, p0.y - p2.y);

    return a.x * b.y - a.y * b.x;
}

// Calculate 2d barycentric coordinates
void barycentric2d(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, NDCPosition position, float3* result)
{
    float area_total = area(p0, p1, p2);

	*result = (float3)(area(position, p1, p2) / area_total, area(p0, position, p2) / area_total, area(p0, p1, position) / area_total);
}

// Calculate (according to Perspective Correct Interpolation) the inverse of the depth at point
Depth depth_at_point(const global Triangle triangle, float3 barycentric)
{
    return 1 / triangle[0][2] * barycentric.x + 1 / triangle[1][2] * barycentric.y + 1 / triangle[2][2] * barycentric.z;
}


// Check if point fits the top\left rule for a certain edge
bool is_contained_top_left(const float2 vec, float weight)
{
	return weight > 0 || (weight == 0 && (vec.y > 0 || (vec.y == 0 && vec.x > 0)));
}

// Check if a point is "in" a triangle, according to the top\left rule
bool is_point_in_triangle(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, float3 barycentric)
{		
	float2 v0 = (float2)(p2.x - p1.x, p2.y - p1.y);
    float2 v1 = (float2)(p0.x - p2.x, p0.y - p2.y);
    float2 v2 = (float2)(p1.x - p0.x, p1.y - p0.y);

    return is_contained_top_left(v0, barycentric.x) && is_contained_top_left(v1, barycentric.y) && is_contained_top_left(v2, barycentric.z);
}

bool is_queue_ended(global const Index** queues, uint* indices, uint index)
{
    return !queues[index][indices[index]] && indices[index];
}

bool is_queue_valid(global const Index** queues, uint* indices, uint index, uint queue_size)
{
    return indices[index] < queue_size && !is_queue_ended(queues, indices, index);
}

// Pick the next non-empty bin queue
uint pick_queue(global const Index** queues, uint* indices, const uint work_group_count, const uint queue_size)
{
    uint current_queue = work_group_count;

    for (uint i = 0; i < work_group_count; ++i)
    {
        if (is_queue_valid(queues, indices, i, queue_size))
        {
            if (current_queue == work_group_count)
            {
                current_queue = i;
            }
            else if (queues[i][indices[i]] < queues[current_queue][indices[current_queue]])
            {
                current_queue = i;
            }
        }
    }

    return current_queue;
}

// Check if a triangle is oriented counter clock-wise
bool is_ccw(const global Triangle* triangle, Index idx)
{
    NDCPosition p0, p1, p2;

    p0.x = triangle[idx][0][0];
	p0.y = triangle[idx][0][1];

    p1.x = triangle[idx][1][0];
	p1.y = triangle[idx][1][1];

    p2.x = triangle[idx][2][0];
	p2.y = triangle[idx][2][1];

    return area(p0, p1, p2) <= 0;
}

kernel void fine_rasterize(
    const global Triangle* triangle_data,
    const global Index* bin_queues,
    const ScreenDimension screen_dim,
    const BinQueueConfig config,
    const uint work_group_count,
    global RawColorRGBA* color,
    global Depth* depth)
{
	//DEBUG_ONCE("Fine Rasterizer!\n");
    Fragment current_frag;
    float3 barycentric;
    uint current_queue_elements[MAX_WORK_GROUP_COUNT];
    global const Index* current_queue_bases[MAX_WORK_GROUP_COUNT];

    private NDCPosition current_position_ndc;
    
    const uint x = get_global_id(0);
    const uint y = get_global_id(1);

    const uint bin_count_x = ceil(((float) screen_dim.width) / config.bin_width);
    const uint bin_count_y = ceil(((float) screen_dim.height) / config.bin_height);

    const uint bin_queue_offset = (config.queue_size + 1) * (y * bin_count_x + x);
    const uint elements_per_group = (config.queue_size + 1) * bin_count_x * bin_count_y; 

    uint current_queue;
    Index current_queue_element;
    
    NDCPosition p0, p1, p2;

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

	//if (IS_GLOBAL_HEAD)
	//{
	//	ptrdiff_t consecutive_coords_distance = &triangle_data[0][0][1] - &triangle_data[0][0][0];
	//	ptrdiff_t consecutive_points_distance = &triangle_data[0][1][0] - &triangle_data[0][0][0];
	//	ptrdiff_t consecutive_triangles_distance = &triangle_data[1][0][0] - &triangle_data[0][0][0];
	//
	//	DEBUG_MESSAGE3("coords - %d, points - %d, triangles - %d\n", consecutive_coords_distance, consecutive_points_distance, consecutive_triangles_distance);
	//	global float* triangles_flatten = (global float*) triangle_data;
	//	for (uint i = 0; i < 8 * sizeof(Triangle) / sizeof(float); ++i)
	//	{
	//		DEBUG_MESSAGE1("%f ", triangles_flatten[i]);
	//	}
	//	DEBUG_MESSAGE("\n");
	//}

    while (true)
    {
        current_queue = pick_queue(current_queue_bases, current_queue_elements, work_group_count, config.queue_size);
        
        if (current_queue >= work_group_count) 
        {
            break;
        }
        
        current_queue_element = current_queue_bases[current_queue][current_queue_elements[current_queue]];
		
		p0.x = triangle_data[current_queue_element][0][0];
		p0.y = triangle_data[current_queue_element][0][1];

		p1.x = triangle_data[current_queue_element][1][0];
		p1.y = triangle_data[current_queue_element][1][1];

		p2.x = triangle_data[current_queue_element][2][0];
		p2.y = triangle_data[current_queue_element][2][1];

		//DEBUG_ITEM_SPECIFIC7(1, 1, 0, "Triangle %d - [ (%f, %f), (%f, %f), (%f, %f) ]\n", current_queue_element, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
	
		//DEBUG_MESSAGE9("{%d %d} -> Triangle %d - [ (%f, %f), (%f, %f), (%f, %f) ]\n", x, y, current_queue_element, p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);

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
    const Fragment fragment,
    const ScreenDimension dim,
    global RawColorRGBA* color, global Depth* depth)
{
    shade(fragment, dim, color, depth);
}

kernel void is_point_in_triangle_test(const global Triangle triangle, const ScreenPosition position, const ScreenDimension dim, global bool* result)
{
    NDCPosition pos;
    
    NDCPosition p0, p1, p2;
    
	p0.x = triangle[0][0];
	p0.y = triangle[0][1];

    p1.x = triangle[1][0];
	p1.y = triangle[1][1];

    p2.x = triangle[2][0];
	p2.y = triangle[2][1];
    
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