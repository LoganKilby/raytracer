/* date = October 25th 2021 9:21 am */

#ifndef RAY_LANE_H
#define RAY_LANE_H

#define LANE_WIDTH 1

#if (LANE_WIDTH==8)
#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SSE/NEON optimizations are not available for this compiler
#endif

struct lane_f32
{
    __m256 v;
    
};

struct lane_u32
{
    __m256i v;
};

struct lane_v3
{
    __m256 x;
    __m256 y;
    __m256 z;
};

#elif (LANE_WIDTH==4)
#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SSE/NEON optimizations are not available for this compiler
#endif

struct lane_f32
{
    __m128 v;
    
};

struct lane_u32
{
    __m128i v;
};

struct lane_v3
{
    __m128 x;
    __m128 y;
    __m128 z;
};

#elif (LANE_WIDTH==1)
typedef f32 lane_f32;
typedef u32 lane_u32;
typedef v3 lane_v3;

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result = (lane_f32)a;
    return result;
}

internal void
conditional_assign(lane_u32 *dest, lane_u32 mask, lane_u32 source)
{
    mask = mask ? 0xFFFFFFFF : 0;
    *dest = ((~mask & *dest) | (mask & source));
}

internal void
conditional_assign(lane_f32 *dest, lane_u32 mask, lane_f32 source)
{
    conditional_assign((lane_u32 *)dest, mask, *(lane_u32 *)&source);
}

internal void
conditional_assign(lane_v3 *dest, lane_u32 mask, lane_v3 source)
{
    conditional_assign(&dest->x, mask, source.x);
    conditional_assign(&dest->y, mask, source.y);
    conditional_assign(&dest->z, mask, source.z);
}

internal f32
lane_f32_max(f32 a, f32 b)
{
    lane_f32 result = (a > b) ? a : b;
    return result;
}

internal b32x
mask_is_zeroed(u32 lane_mask)
{
    b32 result = lane_mask == 0;
    return result;
}

internal lane_f32
horizontal_add(lane_f32 a)
{
    f32 result = a;
    return result;
}

internal lane_u32
horizontal_add(lane_u32 a)
{
    u32 result = a;
    return result;
}

internal lane_v3
horizontal_add(lane_v3 a)
{
    v3 result = 
    {
        horizontal_add(a.x),
        horizontal_add(a.y),
        horizontal_add(a.z)
    };
    
    return result;
}

internal lane_f32
lane_max(lane_f32 a, lane_f32 b)
{
    lane_f32 result = (a > b) ? a : b;
    return result;
}
#else
#error Lane width must be set to 1, 4, or 8
#endif

#endif //RAY_LANE_H
