#pragma once


#include "../general/predefs.h"


namespace nr
{

struct ViewPort
{
    NRint x;
    NRint y;

    NRuint w;
    NRuint h;

    void set(const NRint x, const NRint y, const NRuint w, const NRuint h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

struct Color
{
    NRfloat r;
    NRfloat g;
    NRfloat b;
    NRfloat a;

    void set(const NRfloat r, const NRfloat g, const NRfloat b, const NRfloat a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
};

struct RenderState
{
    ViewPort wnd;
    Color color;
};

}
