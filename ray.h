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
    
    u32 bounces_computed;
    u32 tiles_retired;
};

struct work_order
{
    world *world;
    pixel_buffer *buffer;
    u32 min_x;
    u32 min_y;
    u32 max_x;
    u32 max_y;
};

struct work_queue
{
    u32 rays_per_pixel;
    u32 bounce_count;
    u32 work_order_count;
    work_order *work_orders;
    
    volatile u64 bounces_computed;
    volatile u64 next_work_order_index;
    volatile u64 tiles_retired;
};

#endif //RAY_H
