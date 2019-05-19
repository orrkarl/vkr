#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <inc/general/predefs.h>

#include <inc/base/Module.h>
#include <inc/base/Buffer.h>
#include <inc/base/Kernel.h>
#include <inc/kernels/base.cl.h>
#include <inc/kernels/vertex_shading.cl.h>
#include <inc/kernels/bin_rasterizer.cl.h>
#include <inc/kernels/fine_rasterizer.cl.h>
#include <inc/pipeline/VertexShader.h>
#include <inc/pipeline/BinRasterizer.h>
#include <inc/pipeline/FineRasterizer.h>

namespace _nr = nr::__internal;

bool init(const nr::string name, GLFWerrorfun errorCallback, GLFWkeyfun keyCallback, GLFWwindow* wnd);


// Nraster utilities

struct FullPipeline
{
    _nr::VertexShader   vertexShader;
    _nr::BinRasterizer  binRasterizer;
    _nr::FineRasterizer fineRasterizer;

    FullPipeline(_nr::Module module, cl_int* err);
};

_nr::Module mkFullModule(cl_int* err);

nr::FrameBuffer mkFrameBuffer(const NRuint w, const NRuint h, nr::Error* err);

nr::Error setupPipeline(
    FullPipeline pipeline,
    const NRuint dim,
    const NRuint simplexCount, const NRfloat* near, const NRfloat* far,         // Vertex shader
    nr::ScreenDimension screenDim, _nr::BinQueueConfig config,                  // Bin rasterizer
    const NRuint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer);         // Fine rasterizer

