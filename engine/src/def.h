#pragma once

#ifdef GDFEXPORT
    // Exports
    #ifdef _MSC_VER
        #define GDFAPI __declspec(dllexport)
        #else
        #define GDFAPI __attribute__((visibility("default")))
    #endif
#else
// Imports
    #ifdef _MSC_VER
        #define GDFAPI __declspec(dllimport)
        #else
        #define GDFAPI
    #endif
#endif

#if defined _WIN32 || defined _WIN64
    #define OS_WINDOWS
#elif defined __linux__
    #define OS_LINUX
    // because fuck mac users amirite
#endif

#include <stdint.h>
#include <stdbool.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

#if defined(__clang__) || defined(__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "expected u64 to be 8 bytes.");
STATIC_ASSERT(sizeof(i8) == 1, "expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "expected i64 to be 8 bytes.");
STATIC_ASSERT(sizeof(f32) == 4, "expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "expected f64 to be 8 bytes.");