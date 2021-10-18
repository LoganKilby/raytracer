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

inline v3
eye_ray_direction(int row, int col, view_plane vp, f32 vp_distance)
{
    return glm::normalize(v3(vp.pixel_size * (col - vp.width / 2 + 0.5),
                             vp.pixel_size * (row - vp.height / 2 + 0.5),
                             -vp_distance));
}

internal f32
ray_intersect_plane(v3 ray_origin, v3 ray_direction, v3 plane_normal, f32 d)
{
    return 0.0;
}

internal v3
ray_cast(world *world, v3 ray_origin, v3 ray_direction)
{
    v3 result = {};
    f32 hit_distance = FLT_MAX;
    v3 attenuation = v3(1);
    
    for(u32 ray_count = 0; ray_count < 8; ++ray_count)
    {
        v3 hit_origin;
        v3 hit_normal;
        u32 hit_mat_index = 0;
        
        for(u32 plane_index = 0; plane_index < world->plane_count; ++plane_index)
        {
            plane plane = world->planes[plane_index];
            f32 denom = dot(plane.normal, ray_direction);
            if(fabs(denom) > EPSILON)
            {
                f32 t = (-plane.d - dot(plane.normal, ray_origin)) / denom;
                
                if((t > EPSILON) && (t < hit_distance))
                {
                    hit_distance = t;
                    hit_mat_index = plane.material_index;
                    hit_normal = plane.normal;
                }
            }
        }
        
        for(u32 sphere_index = 0; sphere_index < world->sphere_count; ++sphere_index)
        {
            sphere sphere = world->spheres[sphere_index];
            v3 sphere_relative_ray_origin = ray_origin - sphere.origin;
            f32 a = dot(ray_direction, ray_direction);
            f32 b =  2.0f * dot(ray_direction, sphere_relative_ray_origin);
            f32 c = dot(sphere_relative_ray_origin, sphere_relative_ray_origin) - sphere.radius * sphere.radius;
            f32 denom = 2.0f * a;
            f32 descriminant = sqrt(b * b - 4.0f * a * c);
            
            if(descriminant > EPSILON)
            {
                f32 tp = (-b + descriminant) / denom;
                f32 tn = (-b - descriminant) / denom;
                
                f32 t = tp;
                if((tn > EPSILON) && (tn < tp))
                {
                    t = tn;
                }
                
                if((t > EPSILON) && (t < hit_distance))
                {
                    hit_distance = t;
                    hit_mat_index = sphere.material_index;
                    hit_normal = normalize(t * ray_direction + sphere_relative_ray_origin);
                }
            }
        }
        
        if(hit_mat_index)
        {
            material mat = world->materials[hit_mat_index];
            
            result += hadamard(attenuation, mat.emit_color);
            f32 cos_attenuation = glm::max(dot(-ray_direction, hit_normal), 0.0f);
            attenuation = hadamard(attenuation,  cos_attenuation * mat.reflect_color);
            
            ray_origin += hit_distance * ray_direction;;
            
            v3 pure_bounce = normalize(ray_direction - 2.0f * dot(hit_normal, ray_direction) * hit_normal);
            v3 random_bounce = normalize(hit_normal + v3(random_bilateral(), random_bilateral(), random_bilateral()));
            ray_direction = normalize(lerp(random_bounce, mat.scatter, pure_bounce));
        }
        else
        {
            material mat = world->materials[hit_mat_index];
            result += hadamard(attenuation, mat.emit_color);
            break;
        }
    }
    
    return result;
}