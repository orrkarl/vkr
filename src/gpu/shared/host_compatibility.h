#pragma once

#include <array>
#include <stdint.h>
#include <stddef.h>

namespace vkr::detail {

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

} // vkr::detail

using i32 = int32_t;
using u32 = uint32_t;
using f32 = float;

using vec2 = vkr::detail::Vec<f32, 2>;
using vec3 = vkr::detail::Vec<f32, 3>;
using vec4 = vkr::detail::Vec<f32, 4>;

using ivec2 = vkr::detail::Vec<i32, 2>;
using ivec3 = vkr::detail::Vec<i32, 3>;
using ivec4 = vkr::detail::Vec<i32, 4>;

using uvec2 = vkr::detail::Vec<u32, 2>;
using uvec3 = vkr::detail::Vec<u32, 3>;
using uvec4 = vkr::detail::Vec<u32, 4>;
