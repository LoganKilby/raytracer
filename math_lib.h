/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include "math.h"

#include "fwd.hpp"
#include "mat4x4.hpp"
#include "matrix_transform.hpp"
#include "vector_double4_precision.hpp"

typedef glm::vec4 v4;
typedef glm::vec3 v3;
typedef glm::vec2 v2;
typedef glm::highp_dvec4 dv4;
typedef glm::highp_dvec3 dv3;

#define mat4_identity() glm::mat4(1.0f)
#define PI (float)M_PI
#define TWO_PI 2*M_PI
#define EPSILON 10e-6

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

inline int
rand_float()
{
    return (float)rand() / (float)RAND_MAX;
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

#endif //MATH_LIB_H
