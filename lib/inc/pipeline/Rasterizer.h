#pragma once

#include "../general/predefs.h"
#include "Stage.h"

namespace nr
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

    Rasterizer(const NRuint& dimension, Error& err);

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

    Error update(const cl::CommandQueue& queue)
    {
        if (!infoUpdated)
        {
            return updateInfo(queue);
        }
    }

    Error apply(const cl::Buffer& src, const cl::Buffer& dest, const cl::CommandQueue& queue) override;

private:
    NRfloat x, y;

    NRbool infoUpdated = true;
    Info h_info;
    cl::Buffer d_info;

    Error updateInfo(const cl::CommandQueue& queue);
};

}


