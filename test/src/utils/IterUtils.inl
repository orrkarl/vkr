#include <algorithm>

namespace utils {

template <typename Result, class IterableIn>
std::vector<Result> buildVectorFrom(const IterableIn& in) {
    std::vector<Result> ret(in.size());
    std::transform(in.cbegin(), in.cend(), ret.begin(), [](auto e){return Result(e);});
    return ret;
}

}
