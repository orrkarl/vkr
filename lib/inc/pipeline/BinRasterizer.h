#pragma once

#include "../general/predefs.h"
#include "../base/Buffer.h"
#include "../base/Kernel.h"
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

struct NR_SHARED BinRasterizerParams
{
    static NRuint getTotalBinQueueSize(const NRuint workGroupCount, const ScreenDimension& dim, const BinQueueConfig config)
    {
        return workGroupCount * getBinCount(dim, config.binWidth, config.binHeight) * (config.queueSize + 1) * sizeof(NRuint);
    }

    static NRuint getBinCount(const ScreenDimension& dim, const NRuint& binWidth, const NRuint& binHeight)
    {
        NRuint xCount = (NRuint) ceil(((NRfloat) dim.width) / binWidth);
        NRuint yCount = (NRuint) ceil(((NRfloat) dim.height) / binHeight);
        return xCount * yCount;
    }
    
    BinRasterizerParams()
    {
    }

    cl_int init(cl::CommandQueue queue) { return CL_SUCCESS; }

    cl_int load(cl::Kernel kernel);

    // Screen Dimensions
    ScreenDimension dimension;

    // Bin Queues Data
    BinQueueConfig binQueueConfig;
    Buffer binQueues;

    // Simplex data
    Buffer triangleData;
    NRuint triangleCount;

    // Overflow handling
    Buffer hasOverflow;

    Buffer batchIndex;
};

typedef Kernel<BinRasterizerParams> BinRasterizer;

}

}