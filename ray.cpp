#include "ray.h"
#include "shading.h"

internal bool
ray_sphere_intersection(ray r, sphere s, ray_hit *out)
{
    f32 t;
    v3 temp = r.origin - s.origin;
    f32 a = glm::dot(r.direction, r.direction);
    f32 b = (f64)2.0f * glm::dot(temp, r.direction);
    f32 c = glm::dot(temp, temp) - glm::dot(s.radius, s.radius);
    f32 disc = b * b - 4.0f * a * c;
    
    if(disc > 0.0f)
    {
        f32 e = sqrt(disc);
        f32 denom = 2.0f * a;
        
        t = (-b - e) / denom;
        if(t > EPSILON)
        {
            out->tmin = t;
            out->normal = (temp + (t * r.direction)) / s.radius;
            out->local_hit_point = r.origin + t * r.direction;
            return true;
        }
        
        t = (-b + e) / denom;
        if(t > EPSILON)
        {
            out->tmin = t;
            out->normal = (temp + t * r.direction) / s.radius;
            out->local_hit_point = r.origin + t * r.direction;
            return true;
        }
    }
    
    return false;
}

internal bool
ray_plane_intersect(ray r, ray_hit *hit, v3 point, v3 normal)
{
    f32 t = glm::dot((point - r.origin), normal) / glm::dot(r.direction, normal);
    if(t > EPSILON)
    {
        // tmin = t;
        // sr.normal = normal;
        // st.local_hit_point = r.origin + t * r.direction;
        // return true
    }
    
    return false;
}
