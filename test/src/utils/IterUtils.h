#pragma once

#include <array>
#include <vector>

namespace utils {

template <typename Result, class IterableIn>
std::vector<Result> buildVectorFrom(const IterableIn& in);

template <typename Result, size_t Size, class IterableIn>
std::array<Result, Size> buildArrayFrom(const IterableIn& in);

template <size_t GroupSize, typename T>
std::vector<std::array<T, GroupSize>> groupBy(const std::vector<T>& original);

template <typename In, typename UnaryOperation>
auto map(const std::vector<In>& original, UnaryOperation op) -> std::vector<decltype(op(std::declval<In>()))>;

template <size_t SizeOut, class IterableIn>
auto takeCountedFrom(const IterableIn& original) -> std::array<typename IterableIn::value_type, SizeOut>;

} // namespace utils

#include "IterUtils.inl"
