#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef __int128_t int128;
typedef int8 i8;
typedef int16 i16;
typedef int32 i32;
typedef int64 i64;
typedef int128 i128;

typedef int8 bool8;
typedef int16 bool16;
typedef int32 bool32;
typedef int64 bool64;
typedef int128 bool128;
typedef bool8 b8;
typedef bool16 b16;
typedef bool32 b32;
typedef bool64 b64;
typedef bool128 b128;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef __uint128_t uint128;
typedef uint8 u8;
typedef uint16 u16;
typedef uint32 u32;
typedef uint64 u64;
typedef uint128 u128;

typedef float float32;
typedef double float64;
typedef float32 f32;
typedef float64 f64;

#define max(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define min(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

void eprintf(const char *format, ...) {
    va_list vargs;
    va_start(vargs, format);
    vfprintf(stderr, format, vargs);
    va_end(vargs);
}
