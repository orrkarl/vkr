#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/CommandQueue.h"
#include "../base/Kernel.h"
#include "../base/Module.h"

#include "../rendering/Render.h"

namespace nr
{

namespace __internal
{

struct BinQueueConfig
{
    NRuint binWidth;
    NRuint binHeight;
    NRuint queueSize;
};

struct NR_SHARED BinRasterizer : Kernel
{
    static NRuint getTotalBinQueueCount(const NRuint workGroupCount, const ScreenDimension& dim, const BinQueueConfig config)
    {
        return workGroupCount * getBinCount(dim, config.binWidth, config.binHeight) * (config.queueSize + 1) * sizeof(NRuint);
    }

    static NRuint getBinCount(const ScreenDimension& dim, const NRuint& binWidth, const NRuint& binHeight)
    {
        NRuint xCount = (NRuint) ceil(((NRfloat) dim.width) / binWidth);
        NRuint yCount = (NRuint) ceil(((NRfloat) dim.height) / binHeight);
        return xCount * yCount;
    }
    
    BinRasterizer(Module module, cl_status* err = nullptr)
        : Kernel(module, "bin_rasterize", err)
    {
    }

    cl_status load();

    // Screen Dimensions
    ScreenDimension dimension;

    // Bin Queues Data
    BinQueueConfig binQueueConfig;
    Buffer<NRuint> binQueues;

    // Simplex data
    Buffer<NRfloat> triangleData;
    NRuint triangleCount;

    // Overflow handling
    Buffer<NRbool> hasOverflow;

    Buffer<NRuint> batchIndex;
};

}

}