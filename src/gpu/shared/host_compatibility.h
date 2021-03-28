#pragma once

#include <array>
#include <ostream>
#include <stddef.h>
#include <stdint.h>
#include <tuple>

namespace vkr::detail {

template <typename T, size_t Size>
struct Vec;

template <typename T, size_t Size, size_t N, typename... Tail>
void initializeFrom(std::array<T, Size>& data, Vec<T, N> head, Tail&&... tail) {
    if constexpr (N < Size) {
        static_assert(sizeof...(Tail) == Size - N, "Unexpected tail arguments length");
        auto tailArr = std::array<T, Size - N> { std::forward<Tail>(tail)... };
        std::copy(head.data.cbegin(), head.data.cend(), data.begin());
        std::copy(tailArr.cbegin(), tailArr.cend(), data.begin() + N);
    } else {
        static_assert(sizeof...(Tail) == 0, "Unexpected tail arguments length");
        std::copy(head.data.cbegin(), head.data.cbegin() + Size, data.begin());
    }
}

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif // __GNUC__
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpedantic"
#endif // __clang__

#pragma pack(push, 1)
template <typename T>
struct Vec<T, 2> {
    static constexpr auto Size = 2;
    using Type = T;

    Vec() : data {} {
    }

    template <size_t N, typename... Tail>
    Vec(Vec<T, N> head, Tail&&... tail) {
        initializeFrom(data, head, std::forward<Tail>(tail)...);
    }

    Vec(T x, T y) : x(std::move(x)), y(std::move(y)) {
    }

    explicit Vec(T t) : Vec(t, t) {
    }

    union {
        std::array<T, 2> data;
        struct {
            T x;
            T y;
        };
    };
};
#pragma pack(pop)

#pragma pack(push, 1)
template <typename T>
struct Vec<T, 3> {
    static constexpr auto Size = 3;
    using Type = T;

    Vec() : data {}, padding {} {
    }

    template <size_t N, typename... Tail>
    Vec(Vec<T, N> head, Tail&&... tail) {
        initializeFrom(data, head, std::forward<Tail>(tail)...);
    }

    Vec(T x, T y, T z) : x(std::move(x)), y(std::move(y)), z(std::move(z)), padding {} {
    }

    explicit Vec(T t) : Vec(t, t, t) {
    }

    union {
        std::array<T, 3> data;
        struct {
            T x;
            T y;
            T z;
        };
    };
    std::aligned_storage_t<sizeof(T), alignof(T)> padding;
};
#pragma pack(pop)

#pragma pack(push, 1)
template <typename T>
struct Vec<T, 4> {
    static constexpr auto Size = 4;
    using Type = T;

    Vec() : data {} {
    }

    template <size_t N, typename... Tail>
    Vec(Vec<T, N> head, Tail&&... tail) {
        initializeFrom(data, head, std::forward<Tail>(tail)...);
    }

    Vec(T x, T y, T z, T w) : x(std::move(x)), y(std::move(y)), z(std::move(z)), w(std::move(w)) {
    }

    explicit Vec(T t) : Vec(t, t, t, t) {
    }

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
#pragma pack(pop)

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__
#ifdef __clang__
#pragma clang diagnostic pop
#endif // __clang__

template <typename T, size_t N>
Vec<T, N>& operator+=(Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    for (size_t i = 0; i < N; ++i) {
        lhs.data[i] += rhs.data[i];
    }

    return lhs;
}

template <typename T, size_t N>
Vec<T, N> operator+(const Vec<T, N>& lhs, const Vec<T, N>& rhs) {
    Vec<T, N> ret(lhs);
    ret += rhs;
    return ret;
}

template <typename T, size_t N>
Vec<T, N>& operator*=(Vec<T, N>& v, const T& t) {
    for (T& e : v.data) {
        e *= t;
    }

    return v;
}

template <typename T, size_t N>
Vec<T, N>& operator/=(Vec<T, N>& v, const T& t) {
    for (T& e : v.data) {
        e /= t;
    }

    return v;
}

template <typename T, size_t N>
Vec<T, N> operator*(const Vec<T, N>& v, const T& t) {
    Vec<T, N> ret(v);
    ret *= t;

    return ret;
}

template <typename T, size_t N>
Vec<T, N> operator*(const T& t, const Vec<T, N>& v) {
    return v * t;
}

template <typename T, size_t N>
Vec<T, N> operator/(const Vec<T, N>& v, const T& t) {
    Vec<T, N> ret(v);
    ret /= t;

    return ret;
}

template <typename T, size_t Size>
std::ostream& operator<<(std::ostream& os, const Vec<T, Size>& v) {
    os << "[ ";
    for (size_t i = 0; i < Size - 1; ++i) {
        os << v.data[i] << ", ";
    }
    os << v.data[Size - 1] << " ]";
    return os;
}

template <typename T, size_t Size>
bool operator==(const Vec<T, Size>& v1, const Vec<T, Size>& v2) {
    return v1.data == v2.data;
}

} // namespace vkr::detail

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
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
