#pragma once

#include <vector>

namespace utils {

template <typename Result, class IterableIn>
std::vector<Result> constructFrom(const IterableIn& in);

}

#include "IterUtils.inl"
