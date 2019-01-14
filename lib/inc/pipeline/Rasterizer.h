#pragma once

#include "../general/predefs.h"
#include "Stage.h"

namespace nr
{

namespace __internal
{

class Rasterizer : public Stage
{
public:

    struct Info
    {
        const NRuint dimension;
        NRuint width, height;

        Info(const NRuint width, const NRuint height, const NRuint dimension)
            : width(width), height(height), dimension(dimension)
        {
        }

        Info(const NRuint dimension)
            : Info(0, 0, dimension)
        {
        }
    };

    Rasterizer(const NRuint& dimension, cl_int& err);

    ~Rasterizer()
    {
        
    }

    void set(const NRfloat x, const NRfloat y, const NRfloat width, const NRfloat height)
    {
        this->x = x;
        this->y = y;
        h_info.width = width;
        h_info.height = height;
        infoUpdated = false;
    }

    Error rasterize(
        const cl::Buffer& src,
        const cl::Buffer& dest,
        const cl::CommandQueue& queue,
        const NRuint vertexCount, 
        const Primitive primitive);

private:
    NRfloat x, y;

    NRbool infoUpdated = true;
    Info h_info;
    cl::Buffer d_info;

    cl::Kernel points;
    cl::Kernel lines;
    cl::Kernel simplices;

    Error updateInfo(const cl::CommandQueue& queue);

    Error update(const cl::CommandQueue& queue)
    {
        if (!infoUpdated)
        {
            return updateInfo(queue);
        }
        return Error::NO_ERROR;
    }
};

}

}


