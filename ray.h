/* date = October 4th 2021 8:09 am */

#ifndef RAY_H
#define RAY_H

#include "math_lib.h"
#include "types.h"

global_variable s32 global_entity_id = 0;

struct ray
{
    glm::vec4 origin;
    glm::vec4 direction;
};

struct sphere
{
    v4 origin;
    f32 radius;
    s32 id;
    glm::mat4 transform;
};

struct intersection
{
    f32 t0;
    f32 t1;
    s32 object_id;
};

struct intersection_list
{
    int count;
    // max intersections?
    intersection i[10];
};

inline glm::vec4
ray_position(ray r, f32 dt)
{
    return r.origin + (r.direction - dt);
}

#endif //RAY_H
