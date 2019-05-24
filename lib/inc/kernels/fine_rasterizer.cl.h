#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(

void shade(
    Fragment fragment,
    const ScreenDimension dim,
    RawColorRGB* color, Depth* depth)
{
    uint buffer_index;
    buffer_index = index_from_screen(fragment.position, dim);
        
    if (fragment.depth > depth[buffer_index])
    {
        fragment.color = RAW_RED; // replace this when you get to the actual shading scheme
    
        color[buffer_index] = fragment.color;
        depth[buffer_index] = fragment.depth;
    }
}

float area(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2)
{
    float2 a = (float2)(p1.x - p0.x, p1.y - p0.y);
    float2 b = (float2)(p0.x - p2.x, p0.y - p2.y);

    return a.x * b.y - a.y * b.x;
}

void barycentric2d(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, NDCPosition position, float* result)
{
    float area_total = area(p0, p1, p2);
    
    result[0] = area(position, p1, p2) / area_total;
    result[1] = area(p0, position, p2) / area_total;
    result[2] = 1 - result[0] - result[1];
}

Depth depth_at_point(const generic Triangle triangle, float* barycentric)
{
    return 1 / triangle[0][2] * barycentric[0] + 1 / triangle[1][2] * 1 / barycentric[1] + 1 / triangle[2][2] * barycentric[2];
}

bool is_contained_top_left(const NDCPosition vec, float weight)
{
    return weight > 0 || (weight == 0 && (vec.y > 0 || (vec.y == 0 && vec.x > 0)));
}

bool is_point_in_triangle(const NDCPosition p0, const NDCPosition p1, const NDCPosition p2, float* barycentric)
{
    return is_contained_top_left(p0, barycentric[0]) && is_contained_top_left(p1, barycentric[1]) && is_contained_top_left(p2, barycentric[2]);
}

bool is_queue_ended(const Index** queues, uint* indices, uint index)
{
    return !queues[index][indices[index]] && indices[index];
}

bool is_queue_valid(const Index** queues, uint* indices, uint index, uint queue_size)
{
    return indices[index] < queue_size && !is_queue_ended(queues, indices, index);
}

uint pick_queue(const Index** queues, uint* indices, const uint work_group_count, const uint queue_size)
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

bool is_ccw(const generic Triangle* triangle, Index idx)
{
    NDCPosition p0, p1, p2;
    p0 = (float2)(triangle[idx][0][0], triangle[idx][0][1]);
    p1 = (float2)(triangle[idx][1][0], triangle[idx][1][1]);
    p2 = (float2)(triangle[idx][2][0], triangle[idx][2][1]);

    return area(p0, p1, p2) <= 0;
}

kernel void fine_rasterize(
    const global Triangle* triangle_data,
    const global Index* bin_queues,
    const ScreenDimension screen_dim,
    const BinQueueConfig config,
    const uint work_group_count,
    global RawColorRGB* color,
    global Depth* depth)
{
    DEBUG_ONCE("Starting fine rasterizer\n");

    Fragment current_frag;
    float barycentric[3];
    uint current_queue_elements[MAX_WORK_GROUP_COUNT];
    const Index* current_queue_bases[MAX_WORK_GROUP_COUNT];

    NDCPosition current_position_ndc;
    
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

    while (true)
    {
        current_queue = pick_queue(current_queue_bases, current_queue_elements, work_group_count, config.queue_size);
        
        if (current_queue >= work_group_count) 
        {
            // DEBUG_MESSAGE2("[%d, %d] - work item has finished!\n", x, y);
            break;
        }
        
        current_queue_element = current_queue_bases[current_queue][current_queue_elements[current_queue]];

        DEBUG_MESSAGE3("[%d, %d] - rasterizing triangle %d\n", x, y, current_queue_element);

        if (is_ccw(triangle_data, current_queue_element))
        {
            DEBUG_MESSAGE2("[%d, %d] - current triangle is ccw, ignoring it\n", x, y);
            current_queue_elements[current_queue] += 1;
            continue;
        }

        for (uint frag_x = x * config.bin_width; frag_x < min(screen_dim.width, x * config.bin_width + config.bin_width); ++frag_x)
        {
            for (uint frag_y = y * config.bin_height; frag_y < min(screen_dim.height, y * config.bin_height + config.bin_height); ++frag_y)
            {
                current_frag.position.x = frag_x;
                current_frag.position.y = frag_y;
                
                pixel_mid_point_from_screen(current_frag.position, screen_dim, &current_position_ndc); 
                
                p0 = (float2)(triangle_data[current_queue_element][0][0], triangle_data[current_queue_element][0][1]);
                p1 = (float2)(triangle_data[current_queue_element][1][0], triangle_data[current_queue_element][1][1]);
                p2 = (float2)(triangle_data[current_queue_element][2][0], triangle_data[current_queue_element][2][1]);
    
                barycentric2d(p0, p1, p2, current_position_ndc, barycentric);                

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
    global RawColorRGB* color, global Depth* depth)
{
    shade(fragment, dim, color, depth);
}

kernel void is_point_in_triangle_test(const global Triangle triangle, const ScreenPosition position, const ScreenDimension dim, global bool* result)
{
    NDCPosition pos;
    
    NDCPosition p0, p1, p2;
    
    p0 = (float2)(triangle[0][0], triangle[0][1]);
    p1 = (float2)(triangle[1][0], triangle[1][1]);
    p2 = (float2)(triangle[2][0], triangle[2][1]);
    
    ndc_from_screen(position, dim, &pos);

    float barycentric[3];
    barycentric2d(p0, p1, p2, pos, barycentric);
    *result = is_point_in_triangle(p0, p1, p2, barycentric);
}

#endif // _TEST_FINE

)__CODE__";

}

}

}