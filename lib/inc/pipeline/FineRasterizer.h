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

	FineRasterizer()
		: Kernel()
	{
	}

    cl_status load();

    // Simplex buffer
    Buffer triangleData;

    // Screen Dimensions [size in pixels]
	ScreenDimension dim;

    // Bin Queues
	BinQueueConfig binQueueConfig;
    Buffer binQueues;
	nr_uint workGroupCount;

    // Frame buffer
    FrameBuffer frameBuffer;
};

}

}