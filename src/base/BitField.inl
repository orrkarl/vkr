#pragma once

namespace nr::base {

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType>::BitField(FlagEnumType initialValue)
    : m_value(static_cast<FlagBaseType>(initialValue))
{
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType>::operator FlagBaseType() const
{
    return m_value;
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType>& BitField<FlagEnumType, FlagBaseType>::operator&=(const BitField& other)
{
    m_value &= static_cast<FlagBaseType>(other.m_value);
    return *this;
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType>& BitField<FlagEnumType, FlagBaseType>::operator|=(const BitField& other)
{
    m_value |= static_cast<FlagBaseType>(other.m_value);
    return *this;
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType> BitField<FlagEnumType, FlagBaseType>::operator&(const BitField& other) const
{
    return BitField(m_value & other.m_value);
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType> BitField<FlagEnumType, FlagBaseType>::operator|(const BitField& other) const
{
    return BitField(m_value | other.m_value);
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType> BitField<FlagEnumType, FlagBaseType>::operator~() const
{
    return BitField(FlagEnumType());
}

template <typename FlagEnumType, typename FlagBaseType>
BitField<FlagEnumType, FlagBaseType>::BitField(FlagBaseType rawBitValue)
    : m_value(rawBitValue)
{
}

}