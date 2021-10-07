/* date = October 4th 2021 8:09 am */

#ifndef RAY_H
#define RAY_H

#include "types.h"
#include "shading.h"

global_variable s32 global_entity_id = 0;

struct ray
{
    v4 origin;
    v4 direction;
};

struct sphere
{
    v4 origin;
    f32 radius;
    s32 id;
    glm::mat4 transform;
    material material;
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


#endif //RAY_H
