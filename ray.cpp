#include "ray.h"
#include "shading.h"

internal bool
ray_sphere_intersection(ray r, sphere s, ray_hit *out)
{
    f32 t;
    v3 temp = r.origin - s.origin;
    f32 a = inner(r.direction, r.direction);
    f32 b = 2.0f * inner(temp, r.direction);
    f32 c = inner(temp, temp) - s.radius * s.radius;
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

internal v3
ray_cast(world *world, v3 ray_origin, v3 ray_direction)
{
    v3 result = {};
    f32 hit_distance = FLT_MAX;
    v3 attenuation = V3(1, 1, 1);
    
    for(u32 ray_count = 0; ray_count < 8; ++ray_count)
    {
        v3 hit_origin;
        v3 hit_normal;
        u32 hit_mat_index = 0;
        
        for(u32 plane_index = 0; plane_index < world->plane_count; ++plane_index)
        {
            plane plane = world->planes[plane_index];
            f32 denom = inner(plane.normal, ray_direction);
            if(fabs(denom) > EPSILON)
            {
                f32 t = (-plane.d - inner(plane.normal, ray_origin)) / denom;
                
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
            f32 a = inner(ray_direction, ray_direction);
            f32 b =  2.0f * inner(ray_direction, sphere_relative_ray_origin);
            f32 c = inner(sphere_relative_ray_origin, sphere_relative_ray_origin) - sphere.radius * sphere.radius;
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
            f32 cos_attenuation = max(inner(-ray_direction, hit_normal), 0.0f);
            attenuation = hadamard(attenuation,  cos_attenuation * mat.reflect_color);
            
            ray_origin += hit_distance * ray_direction;;
            
            v3 pure_bounce = normalize(ray_direction - 2.0f * inner(hit_normal, ray_direction) * hit_normal);
            v3 random_bounce = normalize(hit_normal + V3(random_bilateral(), random_bilateral(), random_bilateral()));
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