/* date = October 4th 2021 8:09 am */

#ifndef RAY_H
#define RAY_H

#include "types.h"
#include "shading.h"

global_variable int global_entity_id = 0;

struct ray
{
    v3 origin;
    
    // NOTE: "Ray directions are always (0, 0, -1)"
    v3 direction;
};

struct sphere
{
    v3 origin;
    f32 radius;
    int id;
    material material;
    int material_index;
};

struct plane
{
    v3 origin;
    v3 normal;
    f32 d;
    material material;
    int material_index;
};

struct world
{
    u32 material_count;
    material *materials;
    
    u32 plane_count;
    plane *planes;
    
    u32 sphere_count;
    sphere *spheres;
    
    u32 total_bounces;
};

struct geo
{
    v3 color; // TODO: material
};

struct ray_hit
{
    int object_id;
    f32 t0;
    f32 t1;
    f32 tmin;
    
    v3 local_hit_point;
    v3 normal;
    v4 rgb; // ?
};

struct ray_hit_group
{
    ray_hit *hits;
    int count;
    int max_count;
};

inline v2
ray_screen_coordinates(int col, int row, int width, int height)
{
    v2 result;
    result.x = (col - width / 2.0f) + 0.5f;
    result.y = (row - height / 2.0f) + 0.5f;
    return result;
}

#endif //RAY_H
