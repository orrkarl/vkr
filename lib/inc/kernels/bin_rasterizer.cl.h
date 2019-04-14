#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string bin_rasterizer = R"__CODE__(

// DON'T CHANGE THIS - SOME VARS ARE byte INSTEAD OF int BEACUSE OF THIS SMALL VALUE
#define BATCH_COUNT (256)

global atomic_uint total_batch_index;

typedef struct _Bin
{
    uint width;
    uint height;
    uint x;
    uint y;
} Bin;

bool is_point_in_bin(const NDCPosition point, const Bin bin)
{
    return bin.x <= point.x && point.x <= bin.x + bin.width && bin.y <= point.y && point.y <= bin.y + bin.height; 
}

bool is_simplex_in_bin(const generic NDCPosition simplex[RENDER_DIMENSION], const Bin bin)
{
    bool result = false;
    __attribute__((opencl_unroll_hint))
    for (uchar j = 0; j < RENDER_DIMENSION; ++j)
    {
        result |= is_point_in_bin(simplex[j], bin);
    }
    return result;
}

Bin make_bin(const ScreenDimension dim, const uint index_x, const uint index_y, const uint bin_width, const uint bin_height)
{
    Bin ret;
    ret.x = index_x * bin_width;
    ret.y = index_y * bin_height;
    ret.width = min(dim.width - ret.x, bin_width);
    ret.height = min(dim.height - ret.y, bin_height); 
    return ret;
}

kernel void bin_rasterize(
    const global Simplex* simplex_data,
    const uint total_simplex_count,
    const ScreenDimension dim,
    const uint bin_width,
    const uint bin_height,
    const uint bin_queue_size,
    global atomic_uint* has_overflow,
    global Index* bin_queues)
{
    const local NDCPosition local_simplex_data[BATCH_COUNT * RENDER_DIMENSION];
    local uint current_batch_index;

    private uint group = get_group_id(0);
    private uint index_x = get_local_id(0);
    private uint index_y = get_local_id(1);

    private bool is_init_manager = !index_x && !index_y;

    private event_t batch_acquisition = 0;
    
    private uchar bin_queue_index = 0;

    private uint current_queue_index = bin_queue_size * (ceil(((float) dim.width) / bin_width) * index_y + index_x); 

    private const Bin current_bin = make_bin(dim, index_x, index_y, bin_width, bin_height);

    if (is_init_manager && !group)
    {
        atomic_init(&total_batch_index, 0);
    }

    if (is_init_manager)
    {
        current_batch_index = 0;
    }

    // wait for LOCAL batch index initialization
    work_group_barrier(CLK_LOCAL_MEM_FENCE);

    // wait for GLOBAL batch index initialization
    work_group_barrier(CLK_GLOBAL_MEM_FENCE);

    // queue cleanup
    for (uint i = current_queue_index; i < current_queue_index + bin_queue_size; ++i)
    {
        bin_queues[i] = 0;
    }

    while (true)
    {
        // Aquire a batch (update the local and global batch index)
        if (is_init_manager)
        {
            current_batch_index = atomic_fetch_add(&total_batch_index, BATCH_COUNT) - BATCH_COUNT;
        }

        work_group_barrier(CLK_LOCAL_MEM_FENCE);

        if (atomic_load(has_overflow))
        {
            return;
        }

        // no more batches to process, this work group has no more work to do
        if (current_batch_index >= total_simplex_count) return;

        // Copying x values of each point
        batch_acquisition = async_work_group_strided_copy((local float*) local_simplex_data, (const global float*) simplex_data, BATCH_COUNT * RENDER_DIMENSION, sizeof(Point) / sizeof(float), 0);
        // Copying y values of each point
        async_work_group_strided_copy(((local float*) local_simplex_data) + 1, ((const global float*) simplex_data) + 1, BATCH_COUNT * RENDER_DIMENSION, sizeof(Point) / sizeof(float), batch_acquisition);

        wait_group_events(1, &batch_acquisition);

        for (private uchar i = 0; i <= BATCH_COUNT - 1; ++i)
        {
            if (is_simplex_in_bin(local_simplex_data + i * RENDER_DIMENSION, current_bin))
            {
                bin_queues[current_queue_index++] = i;
            }

            if (current_queue_index >= bin_queue_size)
            {
                atomic_exchange(has_overflow, true);
            }
        }
    }     
}


)__CODE__";

}

}

}