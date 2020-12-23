#pragma once

// If not compiling as device code
#ifndef VKR_COMPILE_DEVICE

#include <array>
#include <stdint.h>
#include <stddef.h>

template <typename T, size_t Size>
struct Vec;

template <typename T>
struct Vec<T, 2> {
    static constexpr auto Size = 2;
    using Type = T;

    union {
        std::array<T, 2> data;
        struct {
            T x;
            T y;
        };
    };
};

template <typename T>
struct Vec<T, 3> {
    static constexpr auto Size = 3;
    using Type = T;

    union {
        std::array<T, 3> data;
        struct {
            T x;
            T y;
            T z;
        };
    };
};

template <typename T>
struct Vec<T, 4> {
    static constexpr auto Size = 4;
    using Type = T;

    union {
        std::array<T, 4> data;
        struct {
            T x;
            T y;
            T z;
            T w;
        };
    };
};

using i32 = int32_t;
using u32 = uint32_t;
using f32 = float;

using vec2 = Vec<f32, 2>;
using vec3 = Vec<f32, 3>;
using vec4 = Vec<f32, 4>;

using ivec2 = Vec<i32, 2>;
using ivec3 = Vec<i32, 3>;
using ivec4 = Vec<i32, 4>;

using uvec2 = Vec<u32, 2>;
using uvec3 = Vec<u32, 3>;
using uvec4 = Vec<u32, 4>;

#else // #ifndef VKR_COMPILE_DEVICE

typedef   int i32;
typedef  uint u32;
typedef float f32;

#endif // #ifndef VKR_COMPILE_DEVICE
