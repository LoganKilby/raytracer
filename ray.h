/* date = October 4th 2021 8:09 am */

#ifndef RAY_H
#define RAY_H

#include "math_functions.h"
#include "types.h"

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
    mat4 transform;
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

inline ray
new_ray(v3 origin, v3 direction)
{
    ray result;
    result.origin = point(origin.x, origin.y, origin.z, 1);
    result.direction = vector(direction.x, direction.y, direction.z, 0);
    return result;
}

inline ray
new_ray(v4 origin, v4 direction)
{
    ray result;
    result.origin = origin;
    result.direction = direction;
    return result;
}

inline sphere
new_sphere(v3 origin, f32 radius)
{
    sphere result;
    result.origin = point(origin.x, origin.y, origin.z, 1);
    result.radius = radius;
    result.id = global_entity_id++;
    result.transform = mat4_identity();
    return result;
}

inline v4
ray_position(ray r, f32 dt)
{
    return v4_add(r.origin, v4_smul(r.direction, dt));
}

#endif //RAY_H
