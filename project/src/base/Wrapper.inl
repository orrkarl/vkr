#pragma once

#include <utility>

#include "Exceptions.h"
#include "Wrapper.h"

namespace nr::base {

template <class WrappedTraits> UniqueWrapper<WrappedTraits>::operator Type() const { return m_object; }

template <class WrappedTraits> bool UniqueWrapper<WrappedTraits>::isValid() const
{
    return m_object != Type();
}

template <class WrappedTraits> UniqueWrapper<WrappedTraits>::~UniqueWrapper()
{
    WrappedTraits::release(m_object);
}

template <class WrappedTraits>
UniqueWrapper<WrappedTraits>& UniqueWrapper<WrappedTraits>::operator=(UniqueWrapper&& other) noexcept
{
    WrappedTraits::release(m_object);
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
    if (!isValid()) {
        throw CLApiException(CL_INVALID_VALUE, "could not create CL handle");
    }
}

template <typename T>
ObjectView<T>::ObjectView(T object)
    : m_rawObject(object)
{
}
template <typename T> T ObjectView<T>::rawHandle() { return m_rawObject; }

}