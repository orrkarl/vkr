#pragma once


#include "../general/predefs.h"


namespace nr
{

struct ViewPort
{
public:
    void set(const NRint x, const NRint y, const NRuint w, const NRuint h)
    {
        glViewport(x, y, w, h);
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }

private:

    NRint x;
    NRint y;

    NRuint w;
    NRuint h;

};

struct Color
{
public:
    void set(const NRfloat r, const NRfloat g, const NRfloat b, const NRfloat a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
        glClearColor(r, g, b, a);
    }

private:
    NRfloat r;
    NRfloat g;
    NRfloat b;
    NRfloat a;
};

struct Depth
{
public:
    void set(const NRfloat z)
    {
        this->z = z;
    }

    void enable()
    {
        glEnable(GL_DEPTH_TEST);
        active = true;
    }

    void disable()
    {
        glDisable(GL_DEPTH_TEST);
        active = false;
    }

    NRbool isEnabled() const
    {
        return active;
    }

private:
    NRbool active = false;
    NRfloat z;
};

struct FaceCulling
{
public:
    enum class Face : NRint
    {
        BACK, FRONT
    };

    enum class Direction : NRint
    {
        CLOCKWISE, COUNTER_CLOCKWISE
    };

    NRbool isActive() const { return active; }

    void activate() 
    {
        active = true;
        glEnable(GL_CULL_FACE);
        glCullFace(fromNRFace(culled));
        glFrontFace(fromNRDirection(direction));
    }

    void deactivate()
    {
        active = false;
        glDisable(GL_CULL_FACE);
    }

    void cullFace(const Face& culled)
    {
        this->culled = culled;
    }

    void frontDirection(const Direction& dir)
    {
        direction = dir;
    }

private:
    Face culled = Face::BACK;
    Direction direction = Direction::CLOCKWISE;
    NRbool active = false;

    static GLenum fromNRFace(const Face& face)
    {
        switch (face)
        {
            case Face::BACK:
                return GL_BACK;
            case Face::FRONT:
                return GL_FRONT;
            default:
                return 0;
        }
    }

    static GLenum fromNRDirection(const Direction& dir)
    {
        switch (dir)
        {
            case Direction::CLOCKWISE:
                return GL_CW;
            case Direction::COUNTER_CLOCKWISE:
                return GL_CCW;
            default:
                return 0;
        }
    }
};

struct RenderState
{
    ViewPort wnd;
    Color color;
    FaceCulling cull;   
};

}
