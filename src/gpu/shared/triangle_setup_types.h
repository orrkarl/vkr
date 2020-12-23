#ifndef _TRIANGLE_SETUP_TYPES_H_
#define _TRIANGLE_SETUP_TYPES_H_

#include "compatibility.h"

struct TriangleParameterBlock {
    /// Z interpolation equation
    u32 depthA, depthB, depthC;

    /// Compressed equations calculating barycentrics
    u32 uCoordA, uCoordB, uCoordC;
    u32 vCoordA, vCoordB, vCoordC;
    u32 wCoordA, wCoordB, wCoordC;

    /// Color at each vertex
    vec4 userParams[3];
};

struct TriangleRasterInfo {
    // Uncompressed edge equations – used to test for triangle\pixel intersection (in sample space)
    i32 A0, B0, C0;
    i32 A1, B1, C1;
    i32 A2, B2, C2;

    // Bounding box of the triangle, in subpixel coordinates
    u16vec2 boundingBoxMin;
    u16vec2 boundingBoxMax;

    TriangleParameterBlock drawParameters;
};

#endif // #ifndef _TRIANGLE_SETUP_TYPES_H_
