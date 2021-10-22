/* date = October 2nd 2021 11:10 pm */

#ifndef TYPES_H
#define TYPES_H

#define internal static
#define local_variable static
#define global_variable static

#include <stdint.h>

typedef float f32;
typedef float lane_f32;
typedef double f64;
typedef int32_t s32;
typedef int64_t s64;
typedef uint32_t u32;
typedef uint32_t lane_u32;
typedef uint64_t u64;
typedef unsigned char u8;
typedef int b32;

#define U32_MAX ((u32)-1)

#endif //TYPES_H
