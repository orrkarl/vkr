#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/Kernel.h"
#include "../rendering/Render.h"
#include "BinRasterizer.h"

namespace nr
{

namespace __internal
{

struct NR_SHARED FineRasterizer : Kernel
{
public:
    FineRasterizer(Module module, cl_status* err = nullptr)
        : Kernel(module, "fine_rasterize", err)
    {
    }

    cl_status init(CommandQueue q) { return CL_SUCCESS; }

    cl_status load(Kernel kernel);

    // Simplex buffer
    Buffer<NRfloat> triangleData;

    // Screen Dimensions [size in pixels]
    ScreenDimension dim;

    // Bin Queues
    BinQueueConfig binQueueConfig;
    Buffer<NRuint> binQueues;
    NRuint workGroupCount;

    // Frame buffer
    FrameBuffer frameBuffer;
};

}

}