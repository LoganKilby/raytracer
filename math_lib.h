/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <float.h>

#define PI (float)M_PI
#define PI32 3.14159265359f
#define TWO_PI 2*M_PI
#define EPSILON 10e-6

#define float_equal(x, y) (fabs(x - y) < EPSILON)
#define float_zero(x) (fabs(x) < EPSILON)
#define square(a) (a * a)

union v3
{
    struct
    {
        f32 x, y, z;
    };
    
    struct
    {
        f32 r, g, b;
    };
};

inline v3
operator*(f32 a, v3 b)
{
    v3 result;
    
    result.x = a*b.x;
    result.y = a*b.y;
    result.z = a*b.z;
    
    return result;
}

inline v3 &
operator*=(v3 &b, f32 a)
{
    b= a * b;
    
    return b;
}

inline v3
operator/(v3 b, f32 a)
{
    v3 result = (1.0f/a)*b;
    
    return result;
}

inline v3 &
operator/=(v3 &b, f32 a)
{
    b= b / a;
    
    return b;
}

inline v3
operator-(v3 a)
{
    v3 result;
    
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    
    return result;
}

inline v3
operator+(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    
    return result;
}

inline v3 &
operator+=(v3 &a, v3 b)
{
    a = a + b;
    
    return(a);
}

inline v3
operator-(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    
    return result;
}

inline v3 &
operator-=(v3 &a, v3 b)
{
    a = a - b;
    
    return a;
}

internal v3
operator*(v3 a, f32 b)
{
    v3 result;
    
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    
    return result;
}

inline f32 
square_root(f32 a)
{
    // TODO(casey): Replace with SSE
    f32 result = (f32)sqrt(a);
    return(result);
}

inline f32
fpow(f32 A, f32 B)
{
    f32 result = (f32)pow(A, B);
    return(result);
}

inline u32
round_f32_to_u32(f32 F)
{
    // TODO(casey): Replace with SSE
    u32 result = (u32)(F + 0.5f);
    return(result);
}

inline v3
V3(f32 a, f32 b, f32 c)
{
    v3 result;
    
    result.x = a;
    result.y = b;
    result.z = c;
    
    return result;
}

inline v3
V3(f32 a)
{
    v3 result;
    
    result.x = a;
    result.y = a;
    result.z = a;
    
    return result;
}

inline f32
lerp(f32 a, f32 t, f32 b)
{
    f32 result = (1.0f - t)*a + t*b;
    
    return result;
}

internal u32
max_dimensions(v3 v)
{
    u32 result;
    
    if(v.x > v.y)
    {
        if(v.x > v.z)
        {
            result = 0;
        }
        else
        {
            result = 2;
        }
    }
    else 
    {
        if(v.y > v.z)
        {
            result = 1;
        }
        else
        {
            result = 2;
        }
    }
    
    return result;
}

inline s32
clamp(s32 min, s32 value, s32 max)
{
    s32 result = value;
    
    if(result < min)
    {
        result = min;
    }
    else if(result > max)
    {
        result = max;
    }
    
    return result;
}

inline f32
clamp(f32 min, f32 value, f32 max)
{
    f32 result = value;
    
    if(result < min)
    {
        result = min;
    }
    else if(result > max)
    {
        result = max;
    }
    
    return result;
}

inline f32
clamp01(f32 value)
{
    f32 result = clamp(0.0f, value, 1.0f);
    
    return result;
}

inline f32
clamp01_map_to_range(f32 min, f32 t, f32 max)
{
    f32 result = 0.0f;
    
    f32 range = max - min;
    if(range != 0.0f)
    {
        result = clamp01((t - min) / range);
    }
    
    return result;
}

inline f32
clamp_above_zero(f32 value)
{
    f32 result = (value < 0) ? 0.0f : value;
    return result;
}

//
// lane_v3 operations
//

inline v3
hadamard(v3 a, v3 b)
{
    v3 result = {a.x*b.x, a.y*b.y, a.z*b.z};
    
    return result;
}

inline f32
inner(v3 a, v3 b)
{
    f32 result = a.x*b.x + a.y*b.y + a.z*b.z;
    return result;
}

inline v3
cross(v3 a, v3 b)
{
    v3 result;
    
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    
    return result;
}

inline f32
length_squared(v3 a)
{
    f32 result = inner(a, a);
    
    return result;
}

inline f32
length(v3 a)
{
    f32 result = square_root(length_squared(a));
    return result;
}

inline v3
normalize(v3 a)
{
    v3 result = a * (1.0f / length(a));
    
    return result;
}

inline v3
noz(v3 a)
{
    v3 result = {};
    
    f32 len_sq = length_squared(a);
    if(len_sq > square(0.0001f))
    {
        result = a * (1.0f / square_root(len_sq));
    }
    
    
    return result;
}

inline v3
clamp01(v3 v)
{
    v3 result;
    
    result.x = clamp01(v.x);
    result.y = clamp01(v.y);
    result.z = clamp01(v.z);
    
    return result;
}

inline v3
lerp(v3 a, f32 t, v3 b)
{
    v3 result = (1.0f - t)*a + t*b;
    
    return result;
}

#include "ray_lane.h"

#endif //MATH_LIb_H
