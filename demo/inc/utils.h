#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

#include <general/predefs.h>

#include <base/Buffer.h>
#include <base/CommandQueue.h>
#include <base/Kernel.h>
#include <base/Module.h>

#include <pipeline/BinRasterizer.h>
#include <pipeline/FineRasterizer.h>
#include <pipeline/SimplexReducer.h>
#include <pipeline/VertexShader.h>

#include "linalg.h"

bool init(const nr::string name, const nr::ScreenDimension& dim, GLFWwindow*& wnd);

// Nraster utilities

struct FullModule : nr::Module
{
    FullModule(cl_status* err);

    cl_status build(const nr_uint dim);

    static const nr::Module::Sources STANDARD_MODULE_KERNELS;
};

struct FullPipeline
{
    nr::__internal::VertexShader   vertexShader;
	nr::__internal::BinRasterizer  binRasterizer;
	nr::__internal::FineRasterizer fineRasterizer;
	nr::__internal::SimplexReducer simplexReducer;

    FullPipeline(FullModule module, cl_status* err);

    cl_status setup(
        const nr_uint dim,
        const nr_uint simplexCount, nr_float* vertecies, nr_float* near, nr_float* far,  // Vertex shader
        nr::ScreenDimension screenDim, nr::__internal::BinQueueConfig config,            // Bin rasterizer
        const nr_uint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer);             // Fine rasterizer

    cl_status operator()(
		nr::CommandQueue q, 
		std::chrono::system_clock::time_point& simplexReducing, 
		std::chrono::system_clock::time_point& vertexShading, 
		std::chrono::system_clock::time_point& binRasterizing, 
		std::chrono::system_clock::time_point& fineRasterizing);

	cl_status operator()(nr::CommandQueue q);

private:
	std::array<nr_size, 1> vertexShaderGlobalSize, vertexShaderLocalSize;
	std::array<nr_size, 1> simplexReducerGlobalSize, simplexReducerLocalSize;
	std::array<nr_size, 2> binRasterizerGlobalSize, binRasterizerLocalSize;
	std::array<nr_size, 2> fineRasterizerGlobalSize, fineRasterizerLocalSize;
};

struct Triangle4d
{
    Vector points[3];

    friend std::ostream& operator<<(std::ostream& os, const Triangle4d& self);
};

struct Tetrahedron
{
    Vector points[4];

    friend std::ostream& operator<<(std::ostream& os, const Tetrahedron& self);
};

// Return a module with all of the code in it
nr::Module mkFullModule(const nr_uint dim, cl_status* err);

// Allocate buffers for a framebuffer
nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_status* err);

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Reduce a 4-Simplex to Triangles
void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4]);

// Reduce a 4-cbue to Triangles
void reduce4Cube(const Vector cube[16], Tetrahedron result[6 * 8 * 4]); 
