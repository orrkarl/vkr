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

struct Vector4d
{
    NRfloat x, y, z, w;

    Vector4d();

    Vector4d(const NRfloat x, const NRfloat y, const NRfloat z, const NRfloat w);

    NRfloat dot(const Vector4d& other) const;

    Vector4d operator-(const Vector4d& other) const;

    bool operator==(const Vector4d& other) const;

    NRfloat distanceSquared(const Vector4d& other) const;

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

_nr::Module mkFullModule(const NRuint dim, cl_int* err);

nr::FrameBuffer mkFrameBuffer(const nr::ScreenDimension& dim, cl_int* err);

void error_callback(int error, const char* description);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void reduce4Simplex(const Tetrahedron& tetrahedron, Triangle4d result[4]);

// 6 tetrahedra for each cube in the 4-cube 3d net, 4 triangles for each tetrahedron
void reduce4Cube(const Vector4d cube[16], Triangle4d result[6 * 8 * 4]); 
