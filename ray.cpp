#include "ray.h"
#include "shading.h"

internal bool
ray_sphere_intersection(ray r, sphere s, ray_hit *out)
{
    f32 t;
    v3 temp = r.origin - s.origin;
    f32 a = glm::dot(r.direction, r.direction);
    f32 b = 2.0f * glm::dot(temp, r.direction);
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

internal void
push_hit(ray_hit_group *group, ray_hit *hit)
{
    Assert(group->count < group->max_count);
    group->hits[group->count++] = *hit;
}

internal void
ray_sphere_intersections(ray r, sphere *spheres, int geo_count, ray_hit_group *out)
{
    f32 t;
    v3 temp;
    f32 a;
    f32 b;
    f32 c;
    f32 disc;
    ray_hit hit;
    
    for(sphere *s = spheres; s < &spheres[0] + geo_count; ++s)
    {
        temp = r.origin - s->origin;
        a = glm::dot(r.direction, r.direction);
        b = 2.0f * glm::dot(temp, r.direction);
        c = glm::dot(temp, temp) - glm::dot(s->radius, s->radius);
        disc = b * b - 4.0f * a * c;
        
        if(disc > 0.0f)
        {
            f32 e = sqrt(disc);
            f32 denom = 2.0f * a;
            
            t = (-b - e) / denom;
            if(t > EPSILON)
            {
                hit.tmin = t;
                hit.normal = (temp + (t * r.direction)) / s->radius;
                hit.local_hit_point = r.origin + t * r.direction;
                hit.rgb = s->material.color;
                push_hit(out, &hit);
            }
            
            t = (-b + e) / denom;
            if(t > EPSILON)
            {
                hit.tmin = t;
                hit.normal = (temp + (t * r.direction)) / s->radius;
                hit.local_hit_point = r.origin + t * r.direction;
                hit.rgb = s->material.color;
                push_hit(out, &hit);
            }
            
            
        }
    }
}

internal bool
ray_plane_intersection(ray r, plane p, ray_hit *out)
{
    f32 t = glm::dot((p.origin - r.origin), p.normal) / glm::dot(r.direction, p.normal);
    if(t > EPSILON)
    {
        out->tmin = t;
        out->normal = p.normal;
        out->local_hit_point = r.origin + t * r.direction;
        out->rgb = p.material.color;
        return true;
    }
    
    return false;
}

internal void
ray_plane_intersections(ray r, plane *planes, int geo_count, ray_hit_group *out)
{
    f32 t;
    ray_hit hit;
    
    for(plane *p = planes; p < &planes[0] + geo_count; ++p)
    {
        t = glm::dot((p->origin - r.origin), p->normal) / glm::dot(r.direction, p->normal);
        if(t > EPSILON)
        {
            hit.tmin = t;
            hit.normal = p->normal;
            hit.local_hit_point = r.origin + t * r.direction;
            hit.rgb = p->material.color;
            push_hit(out, &hit);
        }
    }
}


// TODO: We could sort the hits during insertion into the hit group instead of evaluating them
// like this. Idk which would be better
internal ray_hit
nearest_hit(ray_hit_group *group)
{
    ray_hit result;
    result.tmin = FLT_MAX;
    for(ray_hit *hit = &group->hits[0]; 
        hit < &group->hits[0] + group->count; 
        hit++)
    {
        if(hit->tmin < result.tmin)
            result = *hit;
    }
    
    return result;
}