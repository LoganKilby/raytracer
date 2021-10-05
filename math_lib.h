/* date = October 5th 2021 3:55 pm */

#ifndef MATH_LIB_H
#define MATH_LIB_H

#define _USE_MATH_DEFINES
#include "math.h"

#include "glm.hpp"
#include "matrix_transform.hpp"

typedef glm::vec4 v4;
typedef glm::vec3 v3;

#define mat4_identity() glm::mat4(1.0f)
#define point(x, y, z) glm::vec4(x, y, z, 1)
#define vector(x, y, z) glm::vec4(x, y, z, 0)

#endif //MATH_LIB_H
