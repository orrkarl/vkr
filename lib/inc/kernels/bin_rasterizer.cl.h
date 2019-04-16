#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string bin_rasterizer = R"__CODE__(

// -------------------------------------- Types and defines -------------------------------------- 

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

// ----------------------------------------------------------------------------

// -------------------------------------- Utilities --------------------------------------

bool is_point_in_bin(const float x, const float y, const Bin bin)
{
    return bin.x <= x && x <= bin.x + bin.width && bin.y <= y && y <= bin.y + bin.height; 
}

bool is_simplex_in_bin(const generic float x[RENDER_DIMENSION], const generic float y[RENDER_DIMENSION], const Bin bin)
{
    bool result = false;
    __attribute__((opencl_unroll_hint))
    for (uchar j = 0; j < RENDER_DIMENSION; ++j)
    {
        result |= is_point_in_bin(x[j], y[j], bin);
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

event_t reduce_simplex_buffer(
    const global Simplex* simplices, 
    const uint simplex_count, 
    const uint offset, 
    event_t event, 
    local float* result_x, 
    local float* result_y)
{
    local float* dest_x = (local float*) result_x;
    local float* dest_y = (local float*) result_y;
    const global float* src_base = ((const global float*) simplices) + offset; 

    const uint raw_copy_count = simplex_count * RENDER_DIMENSION; // one for each point in the simplex buffer

    event_t ret = async_work_group_strided_copy(dest_x, src_base, raw_copy_count, RENDER_DIMENSION, 0);      // Copying x values
    return async_work_group_strided_copy(dest_y, src_base + 1, raw_copy_count, RENDER_DIMENSION, ret);   // Copying y values
}

// ----------------------------------------------------------------------------

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
    local float reduced_simplices_x[BATCH_COUNT * RENDER_DIMENSION];
    local float reduced_simplices_y[BATCH_COUNT * RENDER_DIMENSION];
    local uint current_batch_index;
    
    private uint index_x = get_local_id(0);
    private uint index_y = get_local_id(1);

    private bool is_init_manager = !index_x && !index_y;

    private event_t batch_acquisition = 0;
    
    private uchar bin_queue_index = 0;

    private uint bin_queue_base      = get_group_id(0) * bin_queue_size * (ceil(((float) dim.width) / bin_width) * index_y + index_x); 
    private uint current_queue_index = bin_queue_base + 1;

    private const Bin current_bin = make_bin(dim, index_x, index_y, bin_width, bin_height);

    if (get_global_id(0) == 0)
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
        batch_acquisition = reduce_simplex_buffer(simplex_data, BATCH_COUNT, current_batch_index, 0, reduced_simplices_x, reduced_simplices_y);
        wait_group_events(1, &batch_acquisition);

        for (private uint i = 0; i < BATCH_COUNT; ++i)
        {
            if (is_simplex_in_bin(reduced_simplices_x + i * RENDER_DIMENSION, reduced_simplices_y + i * RENDER_DIMENSION, current_bin))
            {
                bin_queues[current_queue_index++] = i;
            }

            if (current_queue_index >= bin_queue_size)
            {
                atomic_exchange(has_overflow, true);
            }
        }

        bin_queues[bin_queue_base] = current_queue_index == bin_queue_base + 1;
        if (current_queue_index != bin_queue_base + 1 + bin_queue_size)
        {
            bin_queues[current_queue_index] = 0;
        }
    }     
}

// -------------------------------------- Tests --------------------------------------

#ifdef _TEST_BINNING

#ifndef SIMPLEX_TEST_COUNT
    #error "SIMPLEX_TEST_COUNT has to be defined in binning test configuration"
#endif 

kernel void is_simplex_in_bin_test(
    const global float simplex_x[RENDER_DIMENSION],
    const global float simplex_y[RENDER_DIMENSION], 
    const uint width, 
    const uint height, 
    const uint x, 
    const uint y,
    global bool* result)
{
    Bin bin = {width, height, x, y};

    *result = is_simplex_in_bin(simplex_x, simplex_y, bin);
}

kernel void reduce_simplex_buffer_test(
    const global Simplex simplex_data[SIMPLEX_TEST_COUNT],
    const uint offset,
    global NDCPosition* result)
{
    local float res_x[SIMPLEX_TEST_COUNT * RENDER_DIMENSION];
    local float res_y[SIMPLEX_TEST_COUNT * RENDER_DIMENSION];
    event_t wait = reduce_simplex_buffer(simplex_data, SIMPLEX_TEST_COUNT, offset, 0, res_x, res_y);
    wait_group_events(1, &wait);

    if (get_global_id(0) == 0)
    {
        __attribute__((opencl_unroll_hint))
        for (uint i = 0; i < SIMPLEX_TEST_COUNT * RENDER_DIMENSION; ++i)
        {
            result[i].x = res_x[i];
            result[i].y = res_y[i];
        }
    }
}

#endif // _TEST_BINNING

)__CODE__";

}

}

}