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
        const nr_uint dim,
        const nr_uint simplexCount, nr_float* vertecies, nr_float* near, nr_float* far,     // Vertex shader
        nr::ScreenDimension screenDim, _nr::BinQueueConfig config,                      // Bin rasterizer
        const nr_uint binRasterWorkGroupCount, nr::FrameBuffer frameBuffer);             // Fine rasterizer

    cl_int operator()(cl::CommandQueue q);
};

struct Vector4d
{
    nr_float x, y, z, w;

    Vector4d();

    Vector4d(const nr_float x, const nr_float y, const nr_float z, const nr_float w);

    nr_float dot(const Vector4d& other) const;

    Vector4d operator-(const Vector4d& other) const;

    bool operator==(const Vector4d& other) const;

    nr_float distanceSquared(const Vector4d& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Vector4d& self);
};

struct Triangle4d
{
    Vector4d points[3];

    friend std::ostream& operator<<(std::ostream& os, const Triangle4d& self);
};

struct Tetrahedron
{
    Vector4d points[4];

    friend std::ostream& operator<<(std::ostream& os, const Tetrahedron& self);
};

// Return a module with all of the code in it
_nr::Module mkFullModule(const nr_uint dim, cl_int* err);

// Allocate buffers for a framebuffer
nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_int* err);

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Reduce a 4-Simplex to Triangles
void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4]);

// Reduce a 4-cbue to Triangles
void reduce4Cube(const Vector4d cube[16], Triangle4d result[6 * 8 * 4]); 
