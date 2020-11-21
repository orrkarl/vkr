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

#define CREATE_ENUM_FLAG_OPS(BitFieldType)                                                                   \
    inline BitFieldType operator|(BitFieldType::EnumType e1, BitFieldType::EnumType e2) {                    \
        return BitFieldType(e1) | BitFieldType(e2);                                                          \
    }                                                                                                        \
    inline BitFieldType operator&(BitFieldType::EnumType e1, BitFieldType::EnumType e2) {                    \
        return BitFieldType(e1) & BitFieldType(e2);                                                          \
    }                                                                                                        \
    inline BitFieldType operator~(BitFieldType::EnumType e) {                                                \
        return ~BitFieldType(e);                                                                             \
    }

}

#include "BitField.inl"