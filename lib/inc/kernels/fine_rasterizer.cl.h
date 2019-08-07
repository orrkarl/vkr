#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* FINE_RASTER_KERNEL_NAME;

class FineRasterizerKernel : public TypesafeKernel<Buffer, Buffer, ScreenDimension, BinQueueConfig, nr_uint, Buffer, Buffer>
{
public:
	FineRasterizerKernel(const Module& module, cl_status* err = nullptr);
};

namespace clcode
{

NRAPI extern const char* fine_rasterizer;

}

}

}