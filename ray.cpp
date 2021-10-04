#include "ray.h"

inline ray
transform_ray(ray r, mat4 m)
{
    mat4 inv_m = mat4_inverse(m);
    ray result;
    result.origin = v4_mat4_multiply(r.origin, inv_m);
    result.direction = v4_mat4_multiply(r.direction, inv_m);
    return result;
}

internal bool
ray_sphere_intersection(ray _ray, sphere s, intersection *out)
{
    ray r = transform_ray(_ray, s.transform);
    
    f32 a = v4_dot(r.direction, r.direction);
    v4 sphere_to_ray = v4_sub(r.origin, s.origin);
    f32 b = 2 * v4_dot(r.direction, sphere_to_ray);
    f32 c = v4_dot(sphere_to_ray, sphere_to_ray) - 1;
    f32 d = b * b - 4 * a * c;
    
    v4 result = {};
    if(d < 0)
    {
        out->t0 = 0;
        out->t1 = 0;
        out->object_id = s.id;
        return false;
    }
    
    f32 sqrt_d = sqrt(d);
    out->t0 = (-b - sqrt_d) / (2 * a);
    out->t1 = (-b + sqrt_d) / (2 * a);
    out->object_id = s.id;
    return true;
}

internal f32
ray_hit(f32 t0, f32 t1)
{
    if(t0 < 0 && t1 < 0)
    {
        return 0;
    }
    else if (t0 >= 0 && t1 >= 0)
    {
        return fmin(t0, t1);
    }
    else
    {
        return fmax(t0, t1);
    }
}

internal void
sort_hits(f32 *t, int count)
{
    // sort by lowest non negative intersection (t)
}

inline ray
ray_translate(ray r, mat4 m)
{
    ray result;
    result.origin = v4_mat4_multiply(r.origin, m);
    result.direction = r.direction;
    return result;
}

inline ray
ray_scale(ray r, mat4 m)
{
    ray result;
    result.origin = v4_mat4_multiply(r.origin, m);
    result.direction = v4_mat4_multiply(r.direction, m);
    return result;
}
