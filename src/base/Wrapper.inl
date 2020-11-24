#pragma once

#include <utility>

#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::UniqueWrapper(Type object)
    : m_object(object) {
    if (!isValid()) {
        throw CLApiException(CL_INVALID_VALUE, "could not create CL handle");
    }
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::UniqueWrapper(const UniqueWrapper& other)
    : m_object(other.m_object) {
    WrappedTraits::retain(m_object);
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>& UniqueWrapper<WrappedTraits>::operator=(const UniqueWrapper& other) {
    WrappedTraits::release(m_object);
    m_object = other.m_object;
    WrappedTraits::retain(m_object);
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::UniqueWrapper(UniqueWrapper&& other) noexcept
    : m_object(other.m_object) {
    other.m_object = Type();
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>& UniqueWrapper<WrappedTraits>::operator=(UniqueWrapper&& other) noexcept {
    WrappedTraits::release(m_object);
    m_object = other.m_object;
    other.m_object = Type();
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::~UniqueWrapper() {
    WrappedTraits::release(m_object);
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::operator Type() const {
    return underlying();
}

template <class WrappedTraits>
typename UniqueWrapper<WrappedTraits>::Type UniqueWrapper<WrappedTraits>::underlying() const {
    return m_object;
}

template <class WrappedTraits>
bool UniqueWrapper<WrappedTraits>::isValid() const {
    return m_object != Type();
}

}