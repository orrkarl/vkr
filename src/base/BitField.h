#pragma once

#include <type_traits>

namespace nr::base {

template <typename FlagEnumType, typename FlagBaseType = std::underlying_type_t<FlagEnumType>>
class BitField {
public:
    using EnumType = FlagEnumType;
    using BaseType = FlagBaseType;

    BitField(FlagEnumType initialValue);

    explicit operator FlagBaseType() const;
    BitField& operator&=(const BitField& other);
    BitField& operator|=(const BitField& other);
    BitField operator&(const BitField& other) const;
    BitField operator|(const BitField& other) const;
    BitField& operator&=(const EnumType& other);
    BitField& operator|=(const EnumType& other);
    BitField operator&(const EnumType& other) const;
    BitField operator|(const EnumType& other) const;
    BitField operator~() const;

private:
    explicit BitField(FlagBaseType rawBitValue);

    FlagBaseType m_value;
};

template <class EnumType>
BitField<EnumType> operator|(EnumType e1, EnumType e2);

template <class EnumType>
BitField<EnumType> operator&(EnumType e1, EnumType e2);

template <class EnumType>
BitField<EnumType> operator~(EnumType e);

}

#include "BitField.inl"