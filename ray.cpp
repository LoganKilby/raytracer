#include "ray.h"
#include "shading.h"

internal u64
xor_shift_64(u64 *a)
{
    // https://en.wikipedia.org/wiki/Xorshift#Example_implementation
    u64 x = *a;
    x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    *a = x;
	return x;
}

internal u32
xor_shift_32(random_series *series)
{
    // https://en.wikipedia.org/wiki/Xorshift#Example_implementation
    u32 x = series->state;
	x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    series->state = x;
    return x;
}

internal f32
random_unilateral(random_series *series)
{
    f32 result = (f32)xor_shift_32(series) / (f32)U32_MAX;
    return result;
}

internal f32
random_bilateral(random_series *series)
{
    f32 result = -1.0f + 2.0f * random_unilateral(series);
    return result;
}

internal u64
locked_add_u64(volatile u64 *addend, u64 value)
{
    u64 result = InterlockedExchangeAdd(addend, value);
    return result;
}

internal void
conditional_assign()
{
    
}


// 1:37:32
internal void
cast_sample_rays(ray_cast_state *state)
{
    // in
    world *world = state->world; 
    u32 rays_per_pixel = state->rays_per_pixel; 
    u32 max_bounce_count = state->max_bounce_count;
    random_series *series = state->series; 
    f32 film_x = state->film_x; 
    f32 film_y = state->film_y; 
    v3 film_center = state->film_center;
    f32 half_film_width = state->half_film_width; 
    f32 half_film_height = state->half_film_height;
    f32 half_pixel_width = state->half_pixel_width; 
    f32 half_pixel_height = state->half_pixel_height;
    v3 camera_x_axis = state->camera_x_axis; 
    v3 camera_y_axis = state->camera_y_axis; 
    v3 camera_position = state->camera_position;
    
    ray r;
    u32 lane_width = 4;
    u32 lane_ray_count = rays_per_pixel / lane_width;
    f32 color_contribution = 1.0f / lane_ray_count;
    u64 bounces_computed = 0;
    v3 final_color = {};
    for(u32 ray_index = 0; ray_index < lane_ray_count; ++ray_index)
    {
        lane_f32 offset_x = film_x + random_bilateral(series) * half_pixel_width;
        lane_f32 offset_y = film_y + random_bilateral(series) * half_pixel_height;
        
        lane_v3 film_position = film_center + camera_x_axis * offset_x * half_film_width + camera_y_axis * offset_y * half_film_height;
        
        r.origin = camera_position;
        r.direction = normalize(film_position - camera_position);
        
        lane_v3 sample = {};
        lane_v3 attenuation = V3(1, 1, 1);
        
        lane_u32 bounces_computed = 0;
        lane_u32 lane_increment = 1;
        lane_u32 lane_mask = 0xFFFFFFFF;
        for(u32 bounce_count = 0; bounce_count < max_bounce_count; ++bounce_count)
        {
            lane_v3 hit_normal = {};
            lane_f32 hit_distance = FLT_MAX;
            lane_u32 hit_mat_index = 0;
            
            bounces_computed += lane_increment;
            
            for(u32 plane_index = 0; plane_index < world->plane_count; ++plane_index)
            {
                plane plane = world->planes[plane_index];
                lane_v3 plane_normal = plane.normal;
                lane_f32 plane_d = plane.d;
                lane_u32 plane_mat_index = plane.material_index;
                
                lane_f32 denom = inner(plane.normal, r.direction);
                lane_f32 t = (-plane_d - inner(plane_normal, r.origin)) / denom;
                
                lane_u32 denom_mask = (denom < -EPSILON) || (denom > EPSILON);
                lane_u32 t_mask = (t > EPSILON) && (t < hit_distance);
                lane_u32 hit_mask = denom_mask & t_mask;
                
                conditional_assign(&hit_distance, hit_mask, t);
                conditional_assign(&hit_mat_index, hit_mask, plane_material_index);
                conditional_assign(&hit_normal, hit_mask, plane.normal);
            }
            
            for(u32 sphere_index = 0; sphere_index < world->sphere_count; ++sphere_index)
            {
                sphere sphere = world->spheres[sphere_index];
                lane_v3 sphere_origin = sphere.origin;
                lane_f32 sphere_radius = sphere.radius;
                lane_u32 sphere_material_index = sphere.material_index;
                lane_v3 sphere_relative_ray_origin = r.origin - sphere_origin;
                lane_f32 a = inner(r.direction, r.direction);
                lane_f32 b =  2.0f * inner(r.direction, sphere_relative_ray_origin);
                lane_f32 c = inner(sphere_relative_ray_origin, sphere_relative_ray_origin) - sphere_radius * sphere_radius;
                lane_f32 denom = 2.0f * a;
                lane_f32 descriminant = square_root(b * b - 4.0f * a * c);
                
                f32 tp = (-b + descriminant) / denom;
                f32 tn = (-b - descriminant) / denom;
                lane_u32 descriminant_mask = (descriminant > EPSILON);
                
                lane_f32 t = tp;
                lane_u32 pick_mask = ((tn > EPSILON) && (tn < tp));
                conditional_assign(&t, pick_mask, tn);
                
                lane_u32 t_mask = ((t > EPSILON) && (t < hit_distance));
                lane_u32 hit_mask = descriminant_mask & t_mask;
                conditional_assign(&hit_distance, hit_mask, t);
                conditional_assign(&hit_mat_index, hit_mask, sphere_material_index);
                conditional_assign(&hit_normal, hit_mask, normalize(t * r.direction + sphere_relative_ray_origin));
            }
            
            if(hit_mat_index)
            {
                material mat = world->materials[hit_mat_index];
                
                sample += hadamard(attenuation, mat.emit_color);
                f32 cos_attenuation = max(inner(-r.direction, hit_normal), 0.0f);
                attenuation = hadamard(attenuation,  cos_attenuation * mat.reflect_color);
                
                r.origin += hit_distance * r.direction;
                
                v3 pure_bounce = normalize(r.direction - 2.0f * inner(hit_normal, r.direction) * hit_normal);
                v3 random_bounce = normalize(hit_normal + V3(random_bilateral(series), 
                                                             random_bilateral(series), 
                                                             random_bilateral(series)));
                r.direction = normalize(lerp(random_bounce, mat.scatter, pure_bounce));
            }
            else
            {
                material mat = world->materials[hit_mat_index];
                sample += hadamard(attenuation, mat.emit_color);
                break;
            }
        }
        
        final_color += color_contribution * sample;
        bounces_computed = bounces_computed;
    }
    
    state->final_color = final_color;
    state->bounces_computed = horizontal_add(bounces_computed);
}

internal bool
render_tile(work_queue *queue)
{
    u32 work_order_index = (u32)locked_add_u64(&queue->next_work_order_index, 1);
    if(work_order_index >= queue->work_order_count)
    {
        return false;
    }
    
    f32 film_distance = 1.0;
    f32 film_width = 1.0;
    f32 film_height = 1.0;
    
    v3 camera_position = V3(0, -10, 1);
    v3 camera_z_axis = normalize(camera_position);
    v3 camera_x_axis = normalize(outer(V3(0, 0, 1), camera_z_axis));
    v3 camera_y_axis = normalize(outer(camera_z_axis, camera_x_axis));
    
    work_order *order = queue->work_orders + work_order_index;
    u32 x_min = order->min_x;
    u32 y_min = order->min_y;
    u32 x_count = order->max_x;
    u32 y_count = order->max_y;
    pixel_buffer *buffer = order->buffer;
    
    if(buffer->width > buffer->height)
    {
        film_height = film_width * (f32)buffer->height / (f32)buffer->width;
    }
    else if(buffer->height > buffer->width)
    {
        film_width = film_height * (f32)buffer->width / (f32)buffer->height;
    }
    
    ray_cast_state state = {};
    state.rays_per_pixel = queue->rays_per_pixel;
    state.max_bounce_count = queue->max_bounce_count;
    state.world = order->world;
    state.series = &order->entropy;
    state.camera_x_axis = camera_x_axis;
    state.camera_y_axis = camera_y_axis;
    state.camera_position = camera_position;
    state.half_film_width = 0.5f * film_width;
    state.half_film_height = 0.5f * film_height;
    state.film_center = camera_position - film_distance * camera_z_axis;
    state.half_pixel_width = 0.5f / buffer->width;
    state.half_pixel_height = 0.5f / buffer->height;
    
    u64 bounces_computed = 0;
    for(u32 row = y_min; row < y_count; ++row)
    {
        state.film_y = -1.0f + 2.0f * ((f32)row / (f32)buffer->height);
        for(u32 col = x_min; col < x_count; ++col)
        {
            state.film_x = -1.0f + 2.0f * ((f32)col / (f32)buffer->width);
            
            cast_sample_rays(&state);
            
            set_pixel(buffer, col, row, V4(state.final_color, 1.0));
            bounces_computed += state.bounces_computed;
        }
    }
    
    locked_add_u64(&queue->tiles_retired, 1);
    locked_add_u64(&queue->bounces_computed, bounces_computed);
    printf("\rrendering... %.0f%%", ((f32)queue->tiles_retired / (f32)queue->work_order_count) * 100);
    fflush(stdout);
    return true;
}


internal DWORD 
WINAPI worker_thread(void *lpParameter)
{
    work_queue *queue = (work_queue *)lpParameter;
    while(render_tile(queue)){};
    return 0;
}

internal void
create_worker_thread(void *param)
{
    DWORD thread_id;
    HANDLE thread_handle = CreateThread(0, 0, worker_thread, param, 0, &thread_id);
    CloseHandle(thread_handle);
}

internal u32
get_core_count()
{
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    
    u32 result = sys_info.dwNumberOfProcessors;
    Assert(result);
    return result;
}
