#pragma once

#include <array>
#include <stddef.h>
#include <stdint.h>

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

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using f32 = float;

using vec2 = vkr::detail::Vec<f32, 2>;
using vec3 = vkr::detail::Vec<f32, 3>;
using vec4 = vkr::detail::Vec<f32, 4>;

using u8vec2 = vkr::detail::Vec<u8, 2>;
using u8vec3 = vkr::detail::Vec<u8, 3>;
using u8vec4 = vkr::detail::Vec<u8, 4>;
using i8vec2 = vkr::detail::Vec<i8, 2>;
using i8vec3 = vkr::detail::Vec<i8, 3>;
using i8vec4 = vkr::detail::Vec<i8, 4>;

using u16vec2 = vkr::detail::Vec<u16, 2>;
using u16vec3 = vkr::detail::Vec<u16, 3>;
using u16vec4 = vkr::detail::Vec<u16, 4>;
using i16vec2 = vkr::detail::Vec<i16, 2>;
using i16vec3 = vkr::detail::Vec<i16, 3>;
using i16vec4 = vkr::detail::Vec<i16, 4>;

using uvec2 = vkr::detail::Vec<u32, 2>;
using uvec3 = vkr::detail::Vec<u32, 3>;
using uvec4 = vkr::detail::Vec<u32, 4>;
using ivec2 = vkr::detail::Vec<i32, 2>;
using ivec3 = vkr::detail::Vec<i32, 3>;
using ivec4 = vkr::detail::Vec<i32, 4>;
