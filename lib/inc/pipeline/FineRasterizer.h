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

class FineRasterizerParams
{
public:
    static NRuint getQuadSize()
    {
        return 4 * sizeof(cl_float) + sizeof(cl_uint) + 2 * sizeof(cl_uint); 
    }

    cl_int init(cl::CommandQueue q) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel);

    // Simplex buffer
    Buffer simplexData;

    // Screen Dimensions [size in pixels]
    ScreenDimension dim;

    // Bin Queues
    BinQueueConfig binQueueConfig;
    Buffer binQueues;

    // Overflow marker
    Buffer overflow;

    // Quad queues
    Buffer quadQueues;
    NRuint quadQueueSize;
};

typedef Kernel<FineRasterizerParams> FineRasterizer;

}

}