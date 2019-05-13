#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(
void apply_fragment(
    const Fragment fragment, const uint buffer_index,
    RawColorRGB* color, Depth* depth, Index* stencil)
{    
    if (depth[buffer_index] > fragment.depth)
    {
        color[buffer_index] = fragment.color;
        stencil[buffer_index] = fragment.stencil;
    }
}

void shade(
    Fragment fragment,
    const ScreenDimension dim,
    RawColorRGB* color, Depth* depth, Index* stencil)
{
    uint buffer_index = index_from_screen(fragment.position, dim);

    fragment.color = RAW_RED; // replace this when you get to the actual shading scheme
    
    apply_fragment(fragment, buffer_index, color, depth, stencil);
}

void barycentric_reduced(const generic Simplex simplex, NDCPosition position, float* result)
{

}

Depth depth_at_point(const generic Simplex simplex, float* barycentric)
{
    Depth ret = 0;
    
    __attribute__((opencl_unroll_hint))
    for (uint i = 0; i < RENDER_DIMENSION; ++i)
    {
        ret += simplex[i][2] * barycentric[i];
    }

    return ret;
}

bool is_point_in_simplex(const NDCPosition position, float* barycentric)
{
    bool ret = true;
    __attribute__((opencl_unroll_hint))
    for (uint i = 0; i < RENDER_DIMENSION - 1; ++i)
    {
        ret &= barycentric[i] >= 0;
    }
    return ret;
}

bool is_queue_ended(const Index* queues, uint* indices, uint index)
{
    return !queues[index][indices] && !index;
}

uint pick_queue(const Index** queues, uint* indices, const uint work_group_count, const uint queue_size)
{
    uint current_queue = work_group_count;
    for (uint i = 0; i < work_group_count; ++i)
    {
        if (current_queue >= work_group_count)
        {
            current_queue = i;
            continue;
        }

        if (indices[i] < queue_size)
        {
            if (queues[i][indices[i]] < queues[current_queue][indices[current_queue]])
            {
                current_queue = i;
            }
        }
    }
    return current_queue;
}

kernel void fine_rasterize(
    const global Simplex* simplex_data,
    const global Index* bin_queues,
    const ScreenDimension screen_dim,
    const BinQueueConfig config,
    const uint work_group_count,
    global RawColorRGB* color,
    global Depth* depth,
    global Index* stencil)
{
    Fragment current_frag;
    float barycentric[RENDER_DIMENSION];
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

    if (work_group_count >= MAX_WORK_GROUP_COUNT) return;

    for (uint i = 0; i < work_group_count; ++i)
    {
        current_queue_bases[i] = bin_queues + bin_queue_offset + i * elements_per_group;
        if (current_queue_bases[0]) current_queue_elements[i] = config.queue_size + 1;
        else current_queue_elements[i] = 0;

        current_queue_bases[i] += 1;
    }

    while (true)
    {
        current_queue = pick_queue(current_queue_bases, current_queue_elements, work_group_count, config.queue_size);
        if (current_queue >= work_group_count) break;
        
        current_queue_element = current_queue_bases[current_queue][current_queue_elements[current_queue]];

        for (uint frag_x = x * config.bin_width; frag_x < min(screen_dim.width, frag_x + config.bin_width); ++frag_x)
        {
            for (uint frag_y = y * config.bin_height; frag_y < min(screen_dim.height, frag_y + config.bin_height); ++frag_y)
            {
                current_frag.position.x = frag_x;
                current_frag.position.y = frag_y;
                
                ndc_from_screen(current_frag.position, screen_dim, &current_position_ndc);

                barycentric_reduced(simplex_data[current_queue_element], current_position_ndc, barycentric);                
                if (is_point_in_simplex(current_position_ndc, barycentric))
                {
                    current_frag.depth = depth_at_point(simplex_data[current_queue_element], barycentric);
                    shade(current_frag, screen_dim, color, depth, stencil);
                }
            }
        }

        current_queue_elements[current_queue] += 1;
    }
}

// -------------------------------------- Tests --------------------------------------

#ifdef _TEST_FINE

kernel void shade_test(
    const global Simplex* simplex_data, const Index simplex_index,
    const Fragment fragment,
    const ScreenDimension dim,
    global RawColorRGB* color, global Depth* depth, global Index* stencil)
{
    shade_test(simplex_data, simplex_index, fragment, dim, color, depth, stencil);
}

kernel void barycentric_reduced_test(const global Simplex simplex, NDCPosition position, global float* result)
{
    barycentric_reduced(simplex, position, result);
}

kernel void is_point_in_simplex_test(const global Simplex simplex, const ScreenPosition position, const ScreenDimension dim, global bool* result)
{
    NDCPosition pos;
    ndc_from_screen(position, dim, &pos);

    float barycentric[RENDER_DIMENSION];
    barycentric_reduced(simplex, pos, barycentric);

    *result = is_point_in_simplex(pos, barycentric);
}

#endif // _TEST_FINE

)__CODE__";

}

}

}