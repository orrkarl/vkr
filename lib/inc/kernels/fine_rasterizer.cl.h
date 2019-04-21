#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string fine_rasterizer = R"__CODE__(

Index* get_next_queue_base(const Index* queue, const uint work_group_count, const uint bin_queue_size, const uint bin_queue_per_group, const uint bin_index)
{
    Index* ret;
    Index* current;

    const uint actual_bin_queue_size = bin_queue_size + 1;
    const uint queue_size = actual_bin_queue_size * bin_queue_per_group;
    const Index* queue_offset = queue + actual_bin_queue_size * bin_index;

    for (uint i = 0; i < work_group_count; ++i)
    {
        current = queue_offset + i * queue_size;
        if (current[0] && (ret == NULL || current[1] > ret[1]))
        {
            ret = current;
        }
    }

    return ret;
}

void apply_quad(const Quad quad, const ScreenDimension dim, RawColorRGB* colorBuffer, Depth* depthBuffer, Index* stencilBuffer)
{
    const uint index = index_from_screen(quad.position, dim);

    colorBuffer[index] = quad.bottom_left.color;
    colorBuffer[index + 1] = quad.top_right.color;
    colorBuffer[index + dim.width] = quad.bottom_left.color;
    colorBuffer[index + dim.width + 1] = quad.top_left.color;
}

void shade(const uint x, const uint y, Quad* dest)
{
    dest->position.x = x;
    dest->position.y = y;
    
    dest->bottom_left.color  = RED;
    dest->top_left.color     = RED;
    dest->top_right.color    = RED;
    dest->bottom_right.color = RED; 
}

void handle_simplex(const generic Simplex simplex, const Bin bin, const ScreenDimension dim, RawColorRGB* colorBuffer, Depth* depthBuffer, Index* stencilBuffer)
{
    Quad current;

    for (uint x = bin.x; x < bin.x + bin.width; x += 2)
    {
        for (uint y = bin.y; y < bin.y + bin.height; y += 2)
        {
            shade(x, y, &current);
            apply_quad(current, dim, colorBuffer, depthBuffer, stencilBuffer);
        }    
    }
}

kernel void fine_rasterize(
    const global Simplex* simplex_data,
    const global Index* bin_queues,
    const ScreenDimension dim,
    const BinQueueConfig config,
    const uint work_group_count,
    global RawColorRGB* color,
    global Depth* depth,
    global Index* stencil)
{
    const uint groupX = get_group_id(0);
    const uint groupY = get_group_id(1);

    const uint x = get_local_id(0) * groupX;
    const uint y = get_local_id(1) * groupY;

    Index* current_queue_base = NULL;

    const uint queues_per_group = ceil(((float) dim.width) / config.bin_width) * ceil(((float) dim.height) / config.bin_height);

    const uint bin_index = y * ceil(((float) dim.width) / config.bin_width) + x;
    const Bin bin = { x * config.bin_width, y * config.bin_height, config.bin_width, config.bin_height };

    private uint current_simplex;

    while (true)
    {
        current_queue_base = get_next_queue_base(bin_queues, work_group_count, config.queue_size, queues_per_group, bin_index);

        // all queues are empty    
        if (!current_queue_base) return;

        do
        {
            handle_simplex(simplex_data[current_queue_base[current_simplex]], bin, dim, color, depth, stencil);
            current_simplex++;
        } while(current_queue_base[current_simplex] != 0);
    }
}

)__CODE__";

}

}

}