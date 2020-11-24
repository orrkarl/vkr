#pragma once

#include "../predefs.h"

#include <vector>

namespace nr::util {

template <typename In, typename Out, typename Transform>
std::vector<Out> extractSizeLimited(const std::vector<In>& elements, Transform transform);

}

#include "Containers.inl"