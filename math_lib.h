/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include "math.h"

#include "fwd.hpp"
#include "mat4x4.hpp"
#include "matrix_transform.hpp"

typedef glm::vec4 v4;
typedef glm::vec3 v3;

#define mat4_identity() glm::mat4(1.0f)
#define point(x, y, z) glm::vec4(x, y, z, 1)
#define color point
#define vector(x, y, z) glm::vec4(x, y, z, 0)
#define PI (float)M_PI
#define TWO_PI 2*M_PI

inline f64 
_phi(f64 x, f64 z)
{
    double phi = atan2(x, z);
    if(phi < 0.0f)
        phi += TWO_PI;
    
    return phi;
}

inline s32
rand_int()
{
    return rand();
}

inline s32
rand_float()
{
    return (float)rand() / (float)RAND_MAX;
}

#endif //MATH_LIB_H
