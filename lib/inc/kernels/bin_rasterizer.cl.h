#pragma once

#include "../general/predefs.h"

#include "../base/Buffer.h"
#include "../base/Kernel.h"

#include "../rendering/Render.h"

namespace nr
{

namespace detail
{

NRAPI extern const char* BIN_RASTER_KERNEL_NAME;
class BinRasterizerKernel : public TypesafeKernel<Buffer, nr_uint, ScreenDimension, BinQueueConfig, Buffer, Buffer, Buffer>
{
public:
	BinRasterizerKernel(const Module& module, cl_status* err = nullptr);
};

namespace clcode
{

NRAPI extern const char* bin_rasterizer;

}

}

}