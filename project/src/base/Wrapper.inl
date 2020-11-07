#pragma once

#include <utility>

namespace nr::base {

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::operator Type() const
{
    return m_object;
}

template <class WrappedTraits>
bool UniqueWrapper<WrappedTraits>::isValid() const
{
    return m_object != Type();
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::~UniqueWrapper()
{
    if (isValid()) {
        WrappedTraits::release(m_object);
    }
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>& UniqueWrapper<WrappedTraits>::operator=(UniqueWrapper&& other) noexcept
{
    if (isValid()) {
        WrappedTraits::release(m_object);
    }

    m_object = other.m_object;
    other.m_object = Type();
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::UniqueWrapper(UniqueWrapper&& other) noexcept
    : m_object(other.m_object)
{
    other.m_object = Type();
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>::UniqueWrapper(Type object)
    : m_object(object)
{
}

}