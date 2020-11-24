#pragma once

#include "../predefs.h"

namespace nr::base {

template <class WrappedTraits>
class UniqueWrapper {
public:
    using Type = typename WrappedTraits::Type;

    UniqueWrapper(Type object);

    UniqueWrapper(const UniqueWrapper& other);
    UniqueWrapper& operator=(const UniqueWrapper& other);

    UniqueWrapper(UniqueWrapper&& other) noexcept;
    UniqueWrapper& operator=(UniqueWrapper&& other) noexcept;

    ~UniqueWrapper();

    operator Type() const;
    Type underlying() const;

    bool isValid() const;

private:
    Type m_object;
};

}

#include "Wrapper.inl"