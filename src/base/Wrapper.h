#pragma once

#include "../predefs.h"

namespace nr::base {

template <class WrappedTraits>
class UniqueWrapper {
public:
    using Type = typename WrappedTraits::Type;

    UniqueWrapper(Type object);

    UniqueWrapper(const UniqueWrapper&) = delete;
    UniqueWrapper& operator=(const UniqueWrapper&) = delete;

    UniqueWrapper(UniqueWrapper&& other) noexcept;
    UniqueWrapper& operator=(UniqueWrapper&& other) noexcept;

    ~UniqueWrapper();

    operator Type() const;
    Type underlying() const;

    bool isValid() const;

private:
    Type m_object;
};

template <typename T>
class ObjectView {
public:
    explicit ObjectView(T object);

    T rawHandle();

protected:
    T m_rawObject;
};

}

#include "Wrapper.inl"