/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include "math.h"
#include <stdlib.h>
#include <float.h>

#define PI (float)M_PI
#define TWO_PI 2*M_PI
#define EPSILON 10e-6

#define float_equal(x, y) (fabs(x - y) < EPSILON)
#define float_zero(x) (fabs(x) < EPSILON)

struct v3
{
    union
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
};

struct v4
{
    union
    {
        struct
        {
            f32 x, y, z, w;
        };
        
        struct
        {
            f32 r, g, b, a;
        };
    };
};

struct v2
{
    f32 x, y;
};

#include "ray_lane.h"

inline v4
V4(f32 a, f32 b, f32 c, f32 d)
{
    v4 result;
    result.x = a;
    result.y = b;
    result.z = c;
    result.w = d;
    return result;
}

inline v4
V4(v3 v, f32 a)
{
    v4 result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    result.w = a;
    return result;
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
hadamard(v3 a, v3 b)
{
    v3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline void
v3_f32_pow(v3 *v, f32 s)
{
    v->x = (f32)pow(v->x, s);
    v->y = (f32)pow(v->y, s);
    v->z = (f32)pow(v->z, s);
}

internal f32
clampf(f32 x, f32 min, f32 max)
{
    if(x >= min && x <= max)
    {
        return x;
    }
    else 
    {
        return x < min ? min : max;
    }
}

inline v3
operator*(v3 b, f32 a)
{
    v3 result;
    result.x = b.x * a;
    result.y = b.y * a;
    result.z = b.z * a;
    return result;
}

inline v3
operator*(f32 a, v3 b)
{
    v3 result;
    result.x = b.x * a;
    result.y = b.y * a;
    result.z = b.z * a;
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

inline v3
operator-(v3 a, v3 b)
{
    v3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

inline v3
operator/(v3 a, f32 b)
{
    v3 result;
    f32 t = 1 / b;
    result.x = a.x * b;
    result.y = a.y * b;
    result.z = a.z * b;
    return result;
}

inline v3 &
operator+=(v3 &a, v3 b)
{
    a = a + b;
    return a;
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
lerp(v3 a, f32 t, v3 b)
{
    v3 result = (1.0f - t)*a + t*b;
    
    return result;
}

inline f32
inner(v3 a, v3 b)
{
    f32 result;
    result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline f32
length_squared(v3 a)
{
    f32 result = inner(a, a);
    return result;
}

inline f32
square_root(f32 a)
{
    f32 result = (f32)sqrt(a);
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
    f32 inv_sqrt = 1 / square_root(length_squared(a));
    v3 result;
    result.x = a.x * inv_sqrt;
    result.y = a.y * inv_sqrt;
    result.z = a.z * inv_sqrt;
    return result;
}

inline v3
noz(v3 a)
{
    v3 result = {};
    f32 len_squared = length_squared(a);
    if(len_squared > (0.0001f * 0.0001f))
    {
        result = a * (1.0f / square_root(len_squared));
    }
    
    return result;
}

inline v3
outer(v3 a, v3 b)
{
    v3 result;
    result.x = a.y*b.z - a.z*b.y;
    result.y = a.z*b.x - a.x*b.z;
    result.z = a.x*b.y - a.y*b.x;
    return result;
}

#endif //MATH_LIB_H
