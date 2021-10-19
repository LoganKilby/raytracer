#include "ray.h"
#include "shading.h"

internal v3
ray_cast(world *world, v3 ray_origin, v3 ray_direction)
{
    v3 result = {};
    f32 hit_distance = FLT_MAX;
    v3 attenuation = V3(1, 1, 1);
    
    for(u32 bounce_count = 0; bounce_count < 8; ++bounce_count)
    {
        ++world->total_bounces;
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

internal void
render_tile(world *world, pixel_buffer *buffer, u32 x_min, u32 y_min, u32 x_count, u32 y_count)
{
    camera camera = {};
    camera.position = V3(0, -10, 1);
    camera.z_axis = normalize(camera.position);
    camera.x_axis = normalize(outer(V3(0, 0, 1), camera.z_axis));
    camera.y_axis = normalize(outer(camera.z_axis, camera.x_axis));
    
    f32 film_distance = 1.0;
    f32 film_width = 1.0;
    f32 film_height = 1.0;
    
    if(buffer->width > buffer->height)
    {
        film_height = film_width * (f32)buffer->height / (f32)buffer->width;
    }
    else if(buffer->height > buffer->width)
    {
        film_width = film_height * (f32)buffer->width / (f32)buffer->height;
    }
    
    f32 half_film_width = 0.5f * film_width;
    f32 half_film_height = 0.5f * film_height;
    v3 film_center = camera.position - film_distance * camera.z_axis;
    
    f32 pixel_width = 0.5f / buffer->width;
    f32 pixel_height = 0.5f / buffer->height;
    
    ray r;
    u32 rays_per_pixel = 1;
    for(int row = y_min; row < y_count; ++row)
    {
        f32 film_y = -1.0 + 2.0 * ((f32)row / (f32)buffer->height);
        for(int col = x_min; col < x_count; ++col)
        {
            f32 film_x = -1.0 + 2.0 * ((f32)col / (f32)buffer->width);
            
            v3 color = {};
            f32 color_contribution = 1.0f / (f32)rays_per_pixel;
            for(u32 ray_index = 0; ray_index < rays_per_pixel; ++ray_index)
            {
                f32 offset_x = film_x + random_bilateral() * pixel_width;
                f32 offset_y = film_y + random_bilateral() * pixel_height;
                
                v3 film_position = film_center + (camera.x_axis * offset_x * half_film_width) + (camera.y_axis * offset_y * half_film_height);
                
                r.origin = camera.position;
                r.direction = normalize(film_position - camera.position);
                
                color += color_contribution * ray_cast(world, r.origin, r.direction);
            }
            
            set_pixel(buffer, col, row, V4(color, 1.0));
        }
    }
}