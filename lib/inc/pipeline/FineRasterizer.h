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

class NR_SHARED FineRasterizerParams
{
public:

    cl_int init(cl::CommandQueue q) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel);

    // Simplex buffer
    Buffer triangleData;

    // Screen Dimensions [size in pixels]
    ScreenDimension dim;

    // Bin Queues
    BinQueueConfig binQueueConfig;
    Buffer binQueues;
    NRuint workGroupCount;

    // Frame buffer
    FrameBuffer frameBuffer;
};

typedef Kernel<FineRasterizerParams> FineRasterizer;

}

}