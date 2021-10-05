#include "ray.h"

internal v4
sphere_normal(glm::mat3 normal_matrix, glm::vec4 w_point)
{
    v3 p = v3(w_point);
    v3 world_normal = p * normal_matrix;
    world_normal = glm::normalize(world_normal);
    return v4(world_normal, 0);
}

internal sphere
new_sphere(v4 origin = v4(0, 0, 0, 1), f32 radius = 1)
{
    sphere s;
    s.origin = origin;
    s.radius = radius;
    s.transform = mat4_identity();
    return s;
}

inline ray
transform_ray(ray r, glm::mat4 m)
{
    glm::mat4 inv_m = glm::inverse(m);
    ray result;
    result.origin = r.origin * inv_m;
    result.direction = r.direction * inv_m;
    return result;
}

internal bool
ray_sphere_intersection(ray _ray, sphere s, intersection *out)
{
    ray r = transform_ray(_ray, s.transform);
    
    f32 a = glm::dot(r.direction, r.direction);
    glm::vec4 sphere_to_ray = r.origin - s.origin;
    f32 b = 2 * glm::dot(r.direction, sphere_to_ray);
    f32 c = glm::dot(sphere_to_ray, sphere_to_ray) - 1;
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
ray_translate(ray r, glm::mat4 m)
{
    ray result;
    result.origin = r.origin * m;
    result.direction = r.direction;
    return result;
}

inline ray
ray_scale(ray r, glm::mat4 m)
{
    ray result;
    result.origin = r.origin * m;
    result.direction = r.direction * m;
    return result;
}


// to find the eye vector, negate the ray's direction vector
// to find the surface->light vector, P->L = L - P