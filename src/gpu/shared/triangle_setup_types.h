#ifndef _TRIANGLE_SETUP_TYPES_H_
#define _TRIANGLE_SETUP_TYPES_H_

#include "compatibility.h"

struct TriangleParameterBlock {
    /// Compressed equations calculating barycentrics
    u32 uCoordA, uCoordB, uCoordC;
    u32 vCoordA, vCoordB, vCoordC;
    u32 wCoordA, wCoordB, wCoordC;

    /// Color at each vertex
    vec4 userParams[3];
};

struct TriangleRasterInfo {
    /// Uncompressed edge equations – used to test for triangle\pixel intersection (in sample space)
    i32 A0, B0, C0;
    i32 A1, B1, C1;
    i32 A2, B2, C2;

    /// Z interpolation equation
    u32 depthA, depthB, depthC;

    /// Bounding box of the triangle, in subpixel coordinates
    u16vec2 boundingBoxMin;
    u16vec2 boundingBoxMax;

    TriangleParameterBlock drawParameters;
};

struct TriangleSetupBlock {
    /**
     * @brief Counting TriangleRasterInfo instances associated with this records original triangle
     *
     * 0 means this triangle was culled
     * 0 < totalTriangleCount < 9 means that this record is valid, and so are
     * 		the first (totalTriangleCount - 1) records in the extra triangles array portion
     * anything above 9 is an invalid value.
    */
    u32 totalTriangleCount;

    TriangleRasterInfo triangleInfo;
};

#endif // #ifndef _TRIANGLE_SETUP_TYPES_H_
