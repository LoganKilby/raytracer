/* date = October 25th 2021 9:21 am */

#ifndef RAY_LANE_H
#define RAY_LANE_H

#if !defined(LANE_WIDTH)
#define LANE_WIDTH 4
#endif

#define extract_f32(a, i) ((f32 *)&a)[i]
#define extract_u32(a, i) ((u32 *)&a)[i]

#if (LANE_WIDTH==16)
#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SSE/NEON optimizations are not available for this compiler
#endif
#include "lane_16x.h"

#elif (LANE_WIDTH==8)
#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SSE/NEON optimizations are not available for this compiler
#endif
#include "lane_8x.h"

#elif (LANE_WIDTH==4)
#if COMPILER_MSVC
#include <intrin.h>
#elif COMPILER_LLVM
#include <x86intrin.h>
#else
#error SSE/NEON optimizations are not available for this compiler
#endif
#include "lane_4x.h"

#elif (LANE_WIDTH==1)

typedef f32 lane_f32;
typedef u32 lane_u32;
typedef v3 lane_v3;

internal lane_v3
operator&(lane_u32 a, lane_v3 b)
{
    lane_v3 result;
    
    // TODO: make this a specific function
    a = a ? 0xFFFFFFFF : 0;
    
    u32 x = a & *(u32*)&b.x;
    u32 y = a & *(u32*)&b.y;
    u32 z = a & *(u32*)&b.z;
    
    result.x = *(f32 *)&x;
    result.y = *(f32 *)&y;
    result.z = *(f32 *)&z;
    
    return result;
}

internal void
conditional_assign(lane_u32 *dest, lane_u32 mask, lane_u32 src)
{
    mask = (mask ? 0xFFFFFFFF : 0);
    *dest = ((~mask & *dest) | (mask & src));
}

internal void
conditional_assign(lane_f32 *Dest, lane_u32 Mask, lane_f32 Source)
{
    conditional_assign((lane_u32 *)Dest, Mask, *(lane_u32 *)&Source);
}

internal lane_f32
fmax(lane_f32 a, lane_f32 b)
{
    lane_f32 result = ((a > b) ? a : b);
    return result;
}

internal lane_f32
fmin(lane_f32 a, lane_f32 b)
{
    lane_f32 result = ((a > b) ? b : a);
    return result;
}

internal u32
horizontal_add(lane_u32 a)
{
    u32 result = a;
    return result;
}

internal f32
horizontal_add(lane_f32 a)
{
    f32 result = a;
    return result;
}

internal b32x
mask_is_zeroed(lane_u32 lane_mask)
{
    b32x result = (lane_mask == 0);
    return result;
}

internal lane_u32
lane_u32_from_u32(lane_u32 a)
{
    lane_u32 result = (lane_u32)a;
    return result;
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3)
{
    lane_u32 result = (lane_u32)r0;
    return result;
}

internal lane_u32
lane_u32_from_u32(u32 r0, u32 r1, u32 r2, u32 r3,
                  u32, u32, u32, u32,
                  u32, u32, u32, u32,
                  u32, u32, u32, u32)
{
    lane_u32 result = (lane_u32)r0;
    return result;
}

internal lane_f32
lane_f32_from_u32(lane_u32 a)
{
    lane_f32 result = (lane_f32)a;
    return result;
}

internal lane_f32
lane_f32_from_f32(f32 a)
{
    lane_f32 result = a;
    return result;
}


internal lane_f32
gather_f32_(void *base_ptr, u32 stride, lane_u32 index)
{
    lane_f32 result;
    result = *(f32 *)((u8 *)base_ptr + index*stride);
    
    return result;
}

internal lane_f32
wabs(lane_f32 a)
{
    lane_u32 b = *(u32 *)&a & (U32_MAX >> 1);
    lane_f32 result = *(f32 *)&b;
    
    return result;
}

#else
#error Lane width must be set to 1, 4, or 8
#endif

#if (LANE_WIDTH != 1)

internal void
conditional_assign(lane_v3 *Dest, lane_u32 Mask, lane_v3 Source);

internal lane_f32 
operator+(lane_f32 a, f32 b)
{
    lane_f32 result;
    
    result = a + lane_f32_from_f32(b);
    return result;
}

internal lane_f32 
operator+(f32 a, lane_f32 b)
{
    lane_f32 result = lane_f32_from_f32(a) + b;
    return result;
}

internal lane_f32 
operator-(lane_f32 a, f32 b)
{
    lane_f32 result = a - lane_f32_from_f32(b);
    return result;
}

internal lane_f32 
operator-(f32 a, lane_f32 b)
{
    lane_f32 result = lane_f32_from_f32(a) - b;
    return result;
}

internal lane_f32 
operator*(lane_f32 a, f32 b)
{
    lane_f32 result = a * lane_f32_from_f32(b);
    return result;
}

internal lane_f32 
operator*(f32 a, lane_f32 b)
{
    lane_f32 result = lane_f32_from_f32(a) * b;
    return result;
}

internal lane_f32 
operator/(lane_f32 a, f32 b)
{
    lane_f32 result = a / lane_f32_from_f32(b);
    return result;
}

internal lane_f32 
operator/(f32 a, lane_f32 b)
{
    lane_f32 result = lane_f32_from_f32(a) / b;
    return result;
}

internal lane_f32
operator+=(lane_f32 &a, lane_f32 b)
{
    a = a + b;
    return a;
}

internal lane_u32
operator+=(lane_u32 &a, lane_u32 b)
{
    a = a + b;
    return a;
}

internal lane_f32
operator-=(lane_f32 &a, lane_f32 b)
{
    a = a - b;
    return a;
}

internal lane_f32
operator*=(lane_f32 &a, lane_f32 b)
{
    a = a * b;
    return a;
}

internal lane_f32
operator/=(lane_f32 &a, lane_f32 b)
{
    a = a / b;
    return a;
}

internal lane_v3
operator&(lane_u32 a, lane_v3 b)
{
    lane_v3 result;
    
    result.x = a & b.x;
    result.y = a & b.y;
    result.z = a & b.z;
    
    return result;
}

internal lane_u32
operator&=(lane_u32 &a, lane_u32 b)
{
    a = a & b;
    return a;
}

internal lane_u32
operator^=(lane_u32 &a, lane_u32 b)
{
    a = a ^ b;
    return a;
}

internal lane_u32
operator|=(lane_u32 &a, lane_u32 b)
{
    a = a | b;
    return a;
}

lane_u32 &lane_u32::
operator=(u32 b)
{
    *this = lane_u32_from_u32(b);
    return *this;
}

lane_f32 &lane_f32::
operator=(f32 b)
{
    *this = lane_f32_from_f32(b);
    return *this;
}

internal lane_f32 
operator-(lane_f32 a)
{
    lane_f32 result = lane_f32_from_f32(0) - a;
    return result;
}

internal lane_u32
operator>(lane_f32 a, f32 b)
{
    lane_u32 result = (a > lane_f32_from_f32(b));
    return result;
}

internal lane_u32
operator>=(lane_f32 a, f32 b)
{
    lane_u32 result = (a >= lane_f32_from_f32(b));
    return result;
}

internal lane_u32
operator>(f32 a, lane_f32 b)
{
    lane_u32 result = (lane_f32_from_f32(a) > b);
    return result;
}

internal lane_u32
operator<(lane_f32 a, f32 b)
{
    lane_u32 result = (a < lane_f32_from_f32(b));
    return result;
}

internal lane_u32
operator<=(lane_f32 a, f32 b)
{
    lane_u32 result = (a <= lane_f32_from_f32(b));
    return result;
}

internal lane_u32
operator<(f32 a, lane_f32 b)
{
    lane_u32 result = (lane_f32_from_f32(a) < b);
    return result;
}

//
// lane_v3 operators
//

inline lane_v3
operator*(f32 a, lane_v3 b)
{
    lane_v3 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;
    
    return result;
}

inline lane_v3
operator*(lane_v3 b, f32 a)
{
    lane_v3 result = a*b;
    
    return result;
}

inline lane_v3 &
operator*=(lane_v3 &b, f32 a)
{
    b= a * b;
    
    return b;
}

inline lane_v3
operator/(lane_v3 b, f32 a)
{
    lane_v3 result = (1.0f/a)*b;
    
    return result;
}

inline lane_v3 &
operator/=(lane_v3 &b, f32 a)
{
    b= b / a;
    
    return b;
}

inline lane_v3
operator-(lane_v3 a)
{
    lane_v3 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    
    return result;
}

inline lane_v3
operator+(lane_v3 a, lane_v3 b)
{
    lane_v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline lane_v3 &
operator+=(lane_v3 &a, lane_v3 b)
{
    a = a + b;
    
    return(a);
}

inline lane_v3
operator-(lane_v3 a, lane_v3 b)
{
    lane_v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline lane_v3 &
operator-=(lane_v3 &a, lane_v3 b)
{
    a = a - b;
    
    return a;
}

internal lane_v3
operator*(lane_v3 a, lane_f32 b)
{
    lane_v3 result;
    
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    
    return result;
}

internal lane_v3
operator*(lane_f32 a, lane_v3 b)
{
    lane_v3 result = b*a;
    return result;
}

internal void
conditional_assign(lane_u32 *dest, lane_u32 mask, lane_u32 src)
{
    *dest = (and_not(mask, *dest) | (mask & src));
}

//
// wide math functions
//

inline lane_v3
hadamard(lane_v3 a, lane_v3 b)
{
    lane_v3 result = {a.x*b.x, a.y*b.y, a.z*b.z};
    
    return result;
}

inline lane_f32
inner(lane_v3 a, lane_v3 b)
{
    lane_f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}

inline lane_v3
cross(lane_v3 a, lane_v3 b)
{
    lane_v3 result;
    
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    
    return result;
}

inline lane_f32
length_squared(lane_v3 a)
{
    lane_f32 result = inner(a, a);
    
    return result;
}

inline lane_f32
length(lane_v3 a)
{
    lane_f32 result = square_root(length_squared(a));
    return result;
}

inline lane_v3
normalize(lane_v3 a)
{
    lane_v3 result = a * (1.0f / length(a));
    
    return result;
}

inline lane_v3
noz(lane_v3 a)
{
    lane_v3 result = {};
    
    lane_f32 len_sq = length_squared(a);
    lane_u32 mask = (len_sq > square(0.0001f));
    conditional_assign(&result, mask, a * (1.0f / square_root(len_sq)));
    
    return result;
}

internal lane_f32
clamp01(lane_f32 value)
{
    lane_f32 result = fmin(fmax(value, lane_f32_from_f32(0.0f)), lane_f32_from_f32(1.0f));
    return result;
}

inline lane_v3
clamp01(lane_v3 v)
{
    lane_v3 result;
    
    result.x = clamp01(v.x);
    result.y = clamp01(v.y);
    result.z = clamp01(v.z);
    
    return result;
}

inline lane_v3
lerp(lane_v3 a, f32 t, lane_v3 b)
{
    lane_v3 result = (1.0f - t)*a + t*b;
    
    return result;
}

inline lane_v3
lerp(lane_v3 a, lane_f32 t, lane_v3 b)
{
    lane_v3 result = (1.0f - t) * a + t * b;
    
    return(result);
}

internal lane_u32
max_dimensions(lane_v3 v)
{
    lane_u32 result;
    
    lane_u32 dim0_mask = (v.x > v.y) & (v.x > v.z);
    lane_u32 dim1_mask = (v.x < v.y) & (v.y > v.z);
    lane_u32 dim2_mask = ((v.x > v.y) & (v.x < v.z)) | (v.x < v.y) & (v.y < v.z);
    
    conditional_assign(&result, dim0_mask, lane_u32_from_u32(0));
    conditional_assign(&result, dim1_mask, lane_u32_from_u32(1));
    conditional_assign(&result, dim2_mask, lane_u32_from_u32(2));
    
    return result;
}

inline lane_v3
LaneV3(lane_f32 x, lane_f32 y, lane_f32 z)
{
    lane_v3 result;
    
    result.x = x;
    result.y = y;
    result.z = z;
    
    return(result);
}

#endif // LANE_WIDTH != 1

#define gather_v3(base_ptr, index, member) \
gather_v3_(&(base_ptr)->member, sizeof(*(base_ptr)), index)

#define gather_f32(base_ptr, index, member) \
gather_f32_(&(base_ptr)->member, sizeof(*(base_ptr)), index)

internal lane_v3
gather_v3_(void *base_ptr, u32 stride, lane_u32 indices)
{
    lane_v3 result;
    
    // TODO: Could write a custom v3 gather for each lane width
    result.x = gather_f32_((f32 *)base_ptr + 0, stride, indices);
    result.y = gather_f32_((f32 *)base_ptr + 1, stride, indices);
    result.z = gather_f32_((f32 *)base_ptr + 2, stride, indices);
    
    return result;
}

internal void
conditional_assign(lane_v3 *Dest, lane_u32 Mask, lane_v3 Source)
{
    conditional_assign(&Dest->x, Mask, Source.x);
    conditional_assign(&Dest->y, Mask, Source.y);
    conditional_assign(&Dest->z, Mask, Source.z);
}

internal lane_v3 
lane_v3_from_v3(v3 a)
{
    lane_v3 result;
    
    result.x = lane_f32_from_f32(a.x);
    result.y = lane_f32_from_f32(a.y);
    result.z = lane_f32_from_f32(a.z);
    
    return result;
}

internal lane_v3
wabs(lane_v3 a)
{
    lane_v3 result;
    
    result.x = wabs(a.x);
    result.y = wabs(a.y);
    result.z = wabs(a.z);
    
    return result;
}

internal v3
extract_lane_0(lane_v3 a)
{
    v3 result;
    
    result.x = *(f32 *)&a.x;
    result.y = *(f32 *)&a.y;
    result.z = *(f32 *)&a.z;
    
    return result;
}

internal v3
horizontal_add(lane_v3 a)
{
    v3 result = 
    {
        horizontal_add(a.x),
        horizontal_add(a.y),
        horizontal_add(a.z),
    };
    
    return result;
}

inline lane_v3
LaneV3(f32 x, f32 y, f32 z)
{
    lane_v3 result;
    
    result.x = lane_f32_from_f32(x);
    result.y = lane_f32_from_f32(y);
    result.z = lane_f32_from_f32(z);
    
    return(result);
}

inline lane_v3
LaneV3(v3 v)
{
    lane_v3 result = LaneV3(v.x, v.y, v.z);;
    
    return(result);
}

global_variable lane_f32 zero_lane_f32 = lane_f32_from_f32(0.0f);

#endif //RAY_LANE_H
