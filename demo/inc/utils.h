#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <general/predefs.h>

#include <base/Module.h>
#include <base/Buffer.h>
#include <base/Kernel.h>
#include <kernels/base.cl.h>
#include <kernels/vertex_shading.cl.h>
#include <kernels/bin_rasterizer.cl.h>
#include <kernels/fine_rasterizer.cl.h>
#include <pipeline/VertexShader.h>
#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>

namespace _nr = nr::__internal;

bool init(const nr::string name, const nr::ScreenDimension& dim, GLFWwindow*& wnd);

// Nraster utilities

struct FullPipeline
{
    _nr::VertexShader   vertexShader;
    _nr::BinRasterizer  binRasterizer;
    _nr::FineRasterizer fineRasterizer;

    FullPipeline(_nr::Module module, cl_int* err);

    cl_int setup(
        const NRuint dim,
        const NRuint simplexCount, NRfloat* vertecies, NRfloat* near, NRfloat* far,     // Vertex shader
        nr::ScreenDimension screenDim, _nr::BinQueueConfig config,                      // Bin rasterizer
        const NRuint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer);             // Fine rasterizer

    cl_int operator()(cl::CommandQueue q);
};

_nr::Module mkFullModule(const NRuint dim, cl_int* err);

nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_int* err);

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

