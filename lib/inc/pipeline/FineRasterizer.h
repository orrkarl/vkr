#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "BinRasterizer.h"

#include "../rendering/Render.h"

namespace nr
{

namespace __internal
{

struct NRAPI FineRasterizer : Kernel
{
public:
    FineRasterizer(Module module, cl_status* err = nullptr)
        : Kernel(module, "fine_rasterize", err)
    {
    }

    cl_status load();

    // Simplex buffer
    Buffer<nr_float> triangleData;

    // Screen Dimensions [size in pixels]
    ScreenDimension dim;

    // Bin Queues
    BinQueueConfig binQueueConfig;
    Buffer<nr_uint> binQueues;
    nr_uint workGroupCount;

    // Frame buffer
    FrameBuffer frameBuffer;
};

}

}