#include <algorithm>
#include <string>

namespace utils {

template <typename Result, class IterableIn>
std::vector<Result> buildVectorFrom(const IterableIn& in) {
    std::vector<Result> ret(in.size());
    std::transform(in.cbegin(), in.cend(), ret.begin(), [](auto e) { return Result(e); });
    return ret;
}

template <typename Result, size_t Size, class IterableIn>
std::array<Result, Size> buildArrayFrom(const IterableIn& in) {
    if (Size != in.size()) {
        throw std::runtime_error("Unexpected sizes in buildArrayFrom: expected " + std::to_string(Size) + ", got "
                                 + std::to_string(in.size()));
    }

    std::array<Result, Size> ret;
    std::transform(in.cbegin(), in.cend(), ret.begin(), [](auto e) { return Result(e); });
    return ret;
}

} // namespace utils
