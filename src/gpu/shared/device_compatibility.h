#ifndef _DEVICE_COMPATIBILITY_H_
#define _DEVICE_COMPATIBILITY_H_

#extension GL_EXT_shader_16bit_storage : require
#extension GL_EXT_shader_8bit_storage : require

#define u8 uint8_t
#define i8 int8_t
#define u16 uint16_t
#define i16 int16_t
#define u32 uint
#define i32 int
#define f32 float

#endif // #ifndef _DEVICE_COMPATIBILITY_H_
