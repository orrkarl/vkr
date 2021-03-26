#pragma once

#include <array>
#include <vector>

namespace utils {

template <typename Result, class IterableIn>
std::vector<Result> buildVectorFrom(const IterableIn& in);

template <typename Result, size_t Size, class IterableIn>
std::array<Result, Size> buildArrayFrom(const IterableIn& in);

} // namespace utils

#include "IterUtils.inl"
