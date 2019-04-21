#pragma once

#include <inc/includes.h>

#include <base/Module.h>
#include <base/Kernel.h>
#include <base/Buffer.h>

#include <kernels/base.cl.h>
#include <kernels/fine_rasterizer.cl.h>

#include <rendering/Render.h>
#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>

#include "fine_utils.h"

using namespace nr;
using namespace nr::__internal;
using namespace testing;

void initEmptyBinQueues(const BinQueueConfig& config, const NRuint workGroupCount[2], const NRuint binCount[2], NRuint* binQueues)
{
    for (NRuint groupY = 0; groupY < workGroupCount[1]; ++groupY)
    {
        for (NRuint groupX = 0; groupX < workGroupCount[0]; ++groupX)
        {
            for (NRuint binY = 0; binY < binCount[1]; ++binY)
            {
                for (NRuint binX = 0; binX < binCount[0]; ++binX)
                {
                    binQueues[config.queueSize * (binCount[0] * binCount[1] * (workGroupCount[0] * groupY + groupX) + binCount[0] * binY + binX)] = 1;   
                }
            }
        }
    }
}

void validateEmptyColorBuffer(const ColorRGB* colorBuffer, const ScreenDimension& dim)
{
    static const ColorRGB BLACK = { 0, 0, 0 };
    for (NRuint i = 0; i < dim.width * dim.height; ++i)
    {
        ASSERT_EQ(BLACK, colorBuffer[i]);
    }
}

TEST(Fine, EmptyQueues)
{
    cl_int err = CL_SUCCESS;
    Error error = Error::NO_ERROR;

    const NRuint dim = 6;
    const ScreenDimension screenDim = { 64, 64 };
    const NRuint workGroupCount[2] = { 2, 2 };
    const BinQueueConfig binQueueConfig = {32, 32, 5};
    const NRuint binQueueCount[2] = 
    {
        (NRuint) ceil(((NRfloat) screenDim.width) / binQueueConfig.binWidth),
        (NRuint) ceil(((NRfloat) screenDim.height) / binQueueConfig.binHeight)
    };

    NRbool h_overflow = false;

    ColorRGB h_colorBuffer[screenDim.width * screenDim.height];

    NRuint h_binQueues[BinRasterizerParams::getTotalBinQueueSize(workGroupCount[0] * workGroupCount[1], screenDim, binQueueConfig) / sizeof(NRuint)];
    initEmptyBinQueues(binQueueConfig, workGroupCount, binQueueCount, h_binQueues);

    const char options_fmt[] = "-cl-std=CL2.0 -Werror -D _DEBUG -D _TEST_FINE -D RENDER_DIMENSION=%d";
    char options[sizeof(options_fmt) * 2];
    memset(options, 0, sizeof(options));
    sprintf(options, options_fmt, dim);

    Module code({clcode::base, clcode::fine_rasterizer}, options, &err);
    ASSERT_EQ(CL_SUCCESS, err);

    cl::CommandQueue q = cl::CommandQueue::getDefault(&err);
    ASSERT_EQ(CL_SUCCESS, err);

    Buffer d_colorBuffer(CL_MEM_WRITE_ONLY, sizeof(h_colorBuffer), &error);
    ASSERT_PRED1(error::isSuccess, error);

    Buffer d_binQueues(CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(h_binQueues), (void*) h_binQueues, &error);
    ASSERT_PRED1(error::isSuccess, error);

    Buffer d_simplexData(CL_MEM_READ_ONLY, sizeof(Simplex<dim>), &error);
    ASSERT_PRED1(error::isSuccess, error);

    FrameBuffer frame;
    frame.color = d_colorBuffer;

    FineRasterizer kernel = code.makeKernel<FineRasterizerParams>("fine_rasterize", &err);
    ASSERT_EQ(CL_SUCCESS, err);

    kernel.params.simplexData = d_simplexData;
    kernel.params.dim = screenDim;
    kernel.params.binQueues = d_binQueues;
    kernel.params.frameBuffer = frame;
    kernel.params.workGroupCountX = workGroupCount[0];
    kernel.params.workGroupCountY = workGroupCount[1]; 

    kernel.global = cl::NDRange(binQueueCount[0], binQueueCount[1]);
    kernel.local  = cl::NDRange(binQueueCount[0] / workGroupCount[0], binQueueCount[1] / workGroupCount[0]);

    ASSERT_EQ(CL_SUCCESS, kernel(q));
    ASSERT_EQ(CL_SUCCESS, q.enqueueReadBuffer(frame.color, CL_FALSE, 0, sizeof(h_colorBuffer), &h_colorBuffer));
    ASSERT_EQ(CL_SUCCESS, q.finish());
    validateEmptyColorBuffer(h_colorBuffer, screenDim);
}

