/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include "math.h"
#include "glm.hpp"

typedef glm::vec4 v4;
typedef glm::vec3 v3;
typedef glm::vec2 v2;
typedef glm::highp_dvec4 dv4;
typedef glm::highp_dvec3 dv3;

#define mat4_identity() glm::mat4(1.0f)
#define PI (float)M_PI
#define TWO_PI 2*M_PI
#define EPSILON 10e-6

#define float_equal(x, y) (fabs(x - y) < EPSILON)
#define float_zero(x) (fabs(x) < EPSILON)

inline f64 
_phi(f64 x, f64 z)
{
    double phi = atan2(x, z);
    if(phi < 0.0f)
        phi += TWO_PI;
    
    return phi;
}

inline int
rand_int()
{
    return rand();
}

inline f32
f32rand()
{
    f32 result = (f32)rand() / (f32)RAND_MAX;
    return result;
}

inline f32
random_unilateral()
{
    f32 result = (f32)rand() / (f32)RAND_MAX;
    return result;
}

inline f32
random_bilateral()
{
    f32 result = -1.0f + 2.0f * random_unilateral();
    return result;
}

inline v3
hadamard(v3 a, v3 b)
{
    return v3(a.x * b.x, a.y * b.y, a.z * b.z);
}

inline void
v3_f32_pow(v3 *v, f32 s)
{
    v->x = glm::pow(v->x, s);
    v->y = glm::pow(v->y, s);
    v->z = glm::pow(v->z, s);
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
lerp(v3 a, f32 t, v3 b)
{
    v3 result = (1.0f - t)*a + t*b;
    
    return result;
}

#endif //MATH_LIB_H
