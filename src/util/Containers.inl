#pragma once

#include <limits>
#include <vector>

#include "../base/Exceptions.h"

namespace nr::util {

template <typename In, typename Out, typename Transform>
std::vector<Out> extractSizeLimited(const std::vector<In>& elements, Transform transform) {
    if (elements.size() >= std::numeric_limits<U32>::max()) {
        throw base::CLApiException(CL_INVALID_VALUE, "too many items in vector");
    }

    std::vector<Out> ret(elements.size());
    std::transform(std::cbegin(elements), std::cend(elements), std::begin(ret), transform);

    return ret;
}

}
