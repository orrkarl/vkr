#pragma once

#include "../general/predefs.h"

namespace nr
{

namespace __internal
{

namespace clcode
{

const string bin_rasterizer = R"__CODE__(

#define BATCH_COUNT (256)

global atomic_uint totalBatchIdx;

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

kernel void bin_rasterize(
    const global Simplex* simplexData,
    uint simplexCount,
    ScreenDimension dim,
    uint binWidth,
    uint binHeight,
    uint binQueueSize,
    global Index* binQueues)
{
    local NDCPosition localSimplexData[BATCH_COUNT * RENDER_DIMENSION];
    local uint currentBatchIdx;

    private uint group = get_group_id(0);
    private uint x = get_local_id(0);
    private uint y = get_local_id(1);

    private bool isInitManager = !x && !y;

    private event_t batchAcquisition;

    if (isInitManager && !group)
    {
        atomic_init(&totalBatchIdx, 0);
    }

    if (isInitManager)
    {
        currentBatchIdx = 0;
    }

    // wait for LOCAL batch index initialization
    work_group_barrier(CLK_LOCAL_MEM_FENCE);

    // wait for GLOBAL batch index initialization
    work_group_barrier(CLK_GLOBAL_MEM_FENCE);

    while (true)
    {
        // Aquire a batch (update the local and global batch index)
        if (isInitManager)
        {
            currentBatchIdx = atomic_fetch_add(&totalBatchIdx, BATCH_COUNT) - BATCH_COUNT;
        }

        work_group_barrier(CLK_LOCAL_MEM_FENCE);

        // no more batches to process, this work group has no more work to do
        if (currentBatchIdx >= simplexCount) break;

        // Copying x values of each point
        batchAcquisition = async_work_group_strided_copy((local float*) localSimplexData, (const global float*) simplexData, BATCH_COUNT * RENDER_DIMENSION, sizeof(Point) / sizeof(float), 0);
        // Copying y values of each point
        async_work_group_strided_copy(((local float*) localSimplexData) + 1, ((const global float*) simplexData) + 1, BATCH_COUNT * RENDER_DIMENSION, sizeof(Point) / sizeof(float), batchAcquisition);

        wait_group_events(1, &batchAcquisition);

        
    }     
}


)__CODE__";

}

}

}