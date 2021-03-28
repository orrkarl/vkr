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

template <size_t GroupSize, typename T>
std::vector<std::array<T, GroupSize>> groupBy(const std::vector<T>& original) {
    if (original.size() % GroupSize != 0) {
        throw std::runtime_error("Unexpected sizes in groupBy: " + std::to_string(original.size())
                                 + " isn't a multiple of " + std::to_string(GroupSize));
    }

    std::vector<std::array<T, GroupSize>> ret(original.size() / GroupSize);
    for (size_t i = 0; i < ret.size(); ++i) {
        std::copy(original.cbegin() + i * GroupSize, original.cbegin() + (i + 1) * GroupSize, ret[i].begin());
    }

    return ret;
}

template <typename In, typename UnaryOperation>
auto map(const std::vector<In>& original, UnaryOperation op) -> std::vector<decltype(op(std::declval<In>()))> {
    std::vector<decltype(op(std::declval<In>()))> ret(original.size());
    std::transform(original.cbegin(), original.cend(), ret.begin(), op);
    return ret;
}

template <size_t SizeOut, class IterableIn>
auto takeCountedFrom(const IterableIn& original) -> std::array<typename IterableIn::value_type, SizeOut> {
    if (SizeOut > original.size()) {
        throw std::runtime_error("Unexpected sizes in take_n: output size " + std::to_string(SizeOut)
                                  + " exceeds input size " + std::to_string(original.size()));
    }

    std::array<typename IterableIn::value_type, SizeOut> ret;
    std::copy_n(original.cbegin(), ret.size(), ret.begin());

    return ret;
}

} // namespace utils
