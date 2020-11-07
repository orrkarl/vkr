#pragma once

#include <type_traits>

namespace nr::base {

template <typename FlagEnumType, typename FlagBaseType>
class BitField {
public:
    BitField(FlagEnumType initialValue);

    operator FlagBaseType() const;
    BitField& operator&=(const BitField& other);
    BitField& operator|=(const BitField& other);
    BitField operator&(const BitField& other) const;
    BitField operator|(const BitField& other) const;
    BitField operator~() const;

private:
    explicit BitField(FlagBaseType rawBitValue);

    FlagBaseType m_value;
};

template <typename FlagEnumType>
using EnumBitField = BitField<FlagEnumType, std::underlying_type_t<FlagEnumType>>;

}

#include "BitField.inl"