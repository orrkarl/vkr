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
    nr_uint binWidth;
    nr_uint binHeight;
    nr_uint queueSize;
};

struct NRAPI BinRasterizer : Kernel
{
    static nr_uint getTotalBinQueueCount(const nr_uint workGroupCount, const ScreenDimension& dim, const BinQueueConfig config);

    static nr_uint getBinCount(const ScreenDimension& dim, const nr_uint& binWidth, const nr_uint& binHeight);
    
    BinRasterizer(Module module, cl_status* err = nullptr);

	BinRasterizer();

    cl_status load();

    // Screen Dimensions
	ScreenDimension dimension;

    // Bin Queues Data
	BinQueueConfig binQueueConfig;
    Buffer binQueues;

    // Simplex data
    Buffer triangleData;
	nr_uint triangleCount;

    // Overflow handling
	Buffer hasOverflow;

    Buffer batchIndex;
};

}

}