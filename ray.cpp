#include "ray.h"
#include "ray_lane.h"

internal u64
locked_add_u64(volatile u64 *addend, u64 value)
{
    u64 result = InterlockedExchangeAdd(addend, value);
    return result;
}

internal u32
locked_exchange_u32(volatile u32 *addend, u32 value)
{
    u32 result = InterlockedExchange(addend, value);
    return result;
}

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

internal lane_u32
xor_shift_32(random_series *series)
{
    // https://en.wikipedia.org/wiki/Xorshift#Example_implementation
    lane_u32 x = series->state;
    x ^= x << 13;
	x ^= x >> 7;
	x ^= x << 17;
    series->state = x;
    return x;
}

internal lane_f32
random_unilateral(random_series *series)
{
    // NOTE: Shifting off the sign bit. There's no SSE instruction to convert to unsigned int
    lane_f32 result = lane_f32_from_u32(xor_shift_32(series) >> 1) / (f32)(U32_MAX >> 1);
    return result;
}

internal lane_f32
random_bilateral(random_series *series)
{
    lane_f32 result = -1.0f + 2.0f * random_unilateral(series);
    return result;
}


global_variable v3 null_brdf_value = { 0, 0, 0 };

internal void
null_brdf(brdf_table *table)
{
    table->count[0] = table->count[1] = table->count[2] = 1;
    table->values = &null_brdf_value;
}

internal void
load_merl_binary(brdf_table *dest, char *file_name)
{
    FILE *file = fopen(file_name, "rb");
    
    if(file)
    {
        fread(dest->count, sizeof(dest->count), 1, file);
        u32 total_count = dest->count[0]* dest->count[1]* dest->count[2];
        u32 total_read_size = total_count * sizeof(f64) * 3;
        u32 total_table_size = total_count * sizeof(v3);
        
        f64 *temp = (f64 *)malloc(total_read_size);
        dest->values = (v3 *)malloc(total_table_size);
        fread(temp, total_read_size, 1, file);
        
        for(u32 data_index = 0; data_index < total_count; ++data_index)
        {
            dest->values[data_index] = { 
                (f32)temp[data_index + 0],
                (f32)temp[total_count + data_index],
                (f32)temp[2 * total_count + data_index],
            };
        }
        
        fclose(file);
        free(temp);
    }
    else
    {
        printf("WARNING: unable to open merl file %s\n", file_name);
    }
}

internal lane_v3
brdf_lookup(material *materials, lane_u32 mat_index, lane_v3 view_dir, lane_v3 normal, lane_v3 tangent, lane_v3 binormal, lane_v3 light_dir)
{
    // calculate a half vector?
    lane_v3 half_vector = noz((view_dir + light_dir) * 0.5f);
    
    lane_v3 light_dir_wide;
    light_dir_wide.x = inner(light_dir, tangent);
    light_dir_wide.y = inner(light_dir, binormal);
    light_dir_wide.z = inner(light_dir, normal);
    
    lane_v3 half_vector_wide;
    half_vector_wide.x = inner(half_vector, tangent);
    half_vector_wide.y = inner(half_vector, binormal);
    half_vector_wide.z = inner(half_vector, normal);
    
    lane_v3 diff_y = noz(cross(half_vector_wide, tangent));
    lane_v3 diff_x = noz(cross(diff_y, half_vector_wide));
    
    lane_f32 diff_x_inner = inner(diff_x, light_dir_wide);
    lane_f32 diff_y_inner = inner(diff_y, light_dir_wide);
    lane_f32 diff_z_inner = inner(half_vector_wide, light_dir_wide);
    
    lane_f32 half_diff = inner(half_vector_wide, light_dir_wide);
    
    lane_v3 result;
    for(u32 sub_index = 0; sub_index < LANE_WIDTH; ++sub_index)
    {
        f32 theta_half = acosf(extract_f32(half_vector_wide.z, sub_index));
        f32 theta_diff = acosf(extract_f32(diff_z_inner, sub_index));
        f32 phi_diff = atan2f(extract_f32(diff_y_inner, sub_index), extract_f32(diff_x_inner, sub_index));
        
        if(phi_diff < 0)
        {
            phi_diff += PI32;
        }
        
        brdf_table *table = &materials[((u32 *)&mat_index)[sub_index]].brdf;
        
        // Undoing what the acos did?
        f32 f0 = square_root(clamp01(theta_half / (0.5f * PI32)));
        u32 i0 = round_f32_to_u32((table->count[0] - 1) * f0);
        
        f32 f1 = clamp01(theta_diff / (0.5f * PI32));
        u32 i1 = round_f32_to_u32((table->count[1] - 1) * f1);
        
        f32 f2 = clamp01(phi_diff / PI32);
        u32 i2= round_f32_to_u32((table->count[2] - 1) * f2);
        
        u32 index = i2 + i1 * table->count[2] + i0 * table->count[1] * table->count[2];
        Assert(index < (table->count[0] * table->count[1] * table->count[2]));
        v3 color = table->values[index];
        
        ((f32 *)&result.x)[sub_index] = color.x;
        ((f32 *)&result.y)[sub_index] = color.y;
        ((f32 *)&result.z)[sub_index] = color.z;
    }
    
    return result;
}

internal void
cast_sample_rays(ray_cast_state *state)
{
    // in
    world *world = state->world; 
    u32 rays_per_pixel = state->rays_per_pixel; 
    u32 max_bounce_count = state->max_bounce_count;
    random_series *series = state->series; 
    lane_v3 film_center = lane_v3_from_v3(state->film_center);
    lane_f32 half_film_width = lane_f32_from_f32(state->half_film_width); 
    lane_f32 half_film_height = lane_f32_from_f32(state->half_film_height);
    lane_f32 half_pixel_width = lane_f32_from_f32(state->half_pixel_width); 
    lane_f32 half_pixel_height = lane_f32_from_f32(state->half_pixel_height);
    lane_f32 film_x = state->film_x + half_pixel_width;
    lane_f32 film_y = state->film_y + half_pixel_height; 
    lane_v3 camera_x_axis = lane_v3_from_v3(state->camera_x_axis); 
    lane_v3 camera_y_axis = lane_v3_from_v3(state->camera_y_axis);
    lane_v3 camera_position = lane_v3_from_v3(state->camera_position);
    
    u32 lane_width = LANE_WIDTH;
    u32 lane_ray_count = rays_per_pixel / lane_width;
    Assert((lane_ray_count *  lane_width) == rays_per_pixel);
    
    f32 color_contribution = 1.0f / rays_per_pixel;
    
    lane_f32 min_hit_distance = lane_f32_from_f32(EPSILON);
    
    // TODO: Could potentially wrap the u32
    lane_u32 bounces_computed = lane_u32_from_u32(0);
    u64 loops_computed = 0;
    lane_v3 final_color = {};
    for(u32 ray_index = 0; ray_index < lane_ray_count; ++ray_index)
    {
        lane_f32 offset_x = film_x + random_bilateral(series) * half_pixel_width;
        lane_f32 offset_y = film_y + random_bilateral(series) * half_pixel_height;
        
        lane_v3 film_position = film_center + camera_x_axis * offset_x * half_film_width + camera_y_axis * offset_y * half_film_height;
        
        lane_v3 ray_origin = camera_position;
        lane_v3 ray_direction = normalize(film_position - camera_position);
        
        lane_v3 sample = {};
        lane_v3 attenuation = V3(1, 1, 1);
        
        lane_u32 lane_mask = lane_u32_from_u32(0xFFFFFFFF);
        for(u32 bounce_count = 0; bounce_count < max_bounce_count; ++bounce_count)
        {
            lane_v3 hit_normal = {};
            //lane_v3 hit_tangent = {};
            //lane_v3 hit_binormal = {};
            lane_f32 hit_distance = lane_f32_from_f32(FLT_MAX);
            lane_u32 hit_mat_index = lane_u32_from_u32(0);
            
            lane_u32 lane_increment = lane_u32_from_u32(1);
            bounces_computed += (lane_increment & lane_mask);
            loops_computed += LANE_WIDTH;
            
            for(u32 plane_index = 0; plane_index < world->plane_count; ++plane_index)
            {
                plane plane = world->planes[plane_index];
                lane_v3 plane_normal = lane_v3_from_v3(plane.normal);
                lane_f32 plane_d = lane_f32_from_f32(plane.d);
#if 0
                lane_v3 plane_tangent = lane_v3_from_v3(plane.tangent);
                lane_v3 plane_binormal = lane_v3_from_v3(plane.binormal);
#endif
                
                lane_f32 denom = inner(plane_normal, ray_direction);
                lane_u32 denom_mask = (denom < -min_hit_distance) | (denom > min_hit_distance);
                
                if(!mask_is_zeroed(denom_mask))
                {
                    lane_f32 t = (-plane_d - inner(plane_normal, ray_origin)) / denom;
                    lane_u32 t_mask = (t > min_hit_distance) & (t < hit_distance);
                    lane_u32 hit_mask = denom_mask & t_mask;
                    
                    if(!mask_is_zeroed(hit_mask))
                    {
                        lane_u32 plane_mat_index = lane_u32_from_u32(plane.material_index);
                        
                        conditional_assign(&hit_distance, hit_mask, t);
                        conditional_assign(&hit_mat_index, hit_mask, plane_mat_index);
                        conditional_assign(&hit_normal, hit_mask, plane_normal);
                        
#if 0
                        conditional_assign(&hit_tangent, hit_mask, plane_tangent);
                        conditional_assign(&hit_binormal, hit_mask, plane_binormal);
#endif
                    }
                }
            }
            
            for(u32 sphere_index = 0; sphere_index < world->sphere_count; ++sphere_index)
            {
                sphere sphere = world->spheres[sphere_index];
                lane_v3 sphere_origin = lane_v3_from_v3(sphere.origin);
                lane_f32 sphere_radius = lane_f32_from_f32(sphere.radius);
                lane_v3 sphere_relative_ray_origin = ray_origin - sphere_origin;
                lane_f32 a = inner(ray_direction, ray_direction);
                lane_f32 b =  2.0f * inner(ray_direction, sphere_relative_ray_origin);
                lane_f32 c = inner(sphere_relative_ray_origin, sphere_relative_ray_origin) - sphere_radius * sphere_radius;
                lane_f32 denom = 2.0f * a;
                lane_f32 descriminant = square_root(b * b - 4.0f * a * c);
                lane_u32 descriminant_mask = (descriminant > EPSILON);
                
                if(!mask_is_zeroed(descriminant_mask))
                {
                    lane_f32 tp = (-b + descriminant) / denom;
                    lane_f32 tn = (-b - descriminant) / denom;
                    
                    lane_f32 t = tp;
                    lane_u32 pick_mask = ((tn > min_hit_distance) & (tn < tp));
                    conditional_assign(&t, pick_mask, tn);
                    
                    lane_u32 t_mask = ((t > EPSILON) & (t < hit_distance));
                    lane_u32 hit_mask = descriminant_mask & t_mask;
                    
                    if(!mask_is_zeroed(hit_mask))
                    {
                        lane_u32 sphere_material_index = lane_u32_from_u32(sphere.material_index);
                        
                        conditional_assign(&hit_distance, hit_mask, t);
                        conditional_assign(&hit_mat_index, hit_mask, sphere_material_index);
                        conditional_assign(&hit_normal, hit_mask, normalize(t * ray_direction + sphere_relative_ray_origin));
                        
#if 0
                        lane_v3 sphere_tangent = cross({0, 0, 1}, hit_normal);
                        lane_v3 sphere_binormal = cross(hit_normal, sphere_tangent);
                        
                        sphere_tangent = noz(sphere_tangent);
                        sphere_binormal = noz(sphere_binormal);
                        
                        conditional_assign(&hit_tangent, hit_mask, sphere_tangent);
                        conditional_assign(&hit_binormal, hit_mask, sphere_binormal);
#endif
                    }
                }
            }
            
            // TODO: n-way load
            //material mat = world->materials[hit_mat_index];
            
            lane_v3 mat_emit_color = lane_mask & gather_v3(world->materials, hit_mat_index, emit_color);
            lane_v3 mat_reflect_color = gather_v3(world->materials, hit_mat_index, reflect_color);
            lane_f32 mat_specular = gather_f32(world->materials, hit_mat_index, specular);
            
            sample += hadamard(attenuation, mat_emit_color);
            lane_mask &= (hit_mat_index != lane_u32_from_u32(0));
            
            if(mask_is_zeroed(lane_mask))
            {
                break;
            }
            else
            {
                lane_f32 cos_attenuation = fmax(inner(-ray_direction, hit_normal), lane_f32_from_f32(0.0f));
                
                ray_origin += hit_distance * ray_direction;
                
                lane_v3 pure_bounce = normalize(ray_direction - 2.0f * inner(hit_normal, ray_direction) * hit_normal);
                lane_v3 random_bounce = noz(hit_normal + LaneV3(random_bilateral(series), 
                                                                random_bilateral(series), 
                                                                random_bilateral(series)));
                lane_v3 next_ray_direction = noz(lerp(random_bounce, mat_specular, pure_bounce));
                
                //lane_v3 reflectance = brdf_lookup(world->materials, hit_mat_index, -ray_direction, hit_normal, hit_tangent, hit_binormal, next_ray_direction);
                
                //attenuation = hadamard(attenuation, reflectance);
                
                attenuation = hadamard(attenuation,  cos_attenuation * mat_reflect_color);
                
                ray_direction = next_ray_direction;
            }
        }
        
        final_color += color_contribution * sample;
    }
    
    state->final_color = horizontal_add(final_color);
    state->bounces_computed += horizontal_add(bounces_computed);
    state->loops_computed += loops_computed;
    state->series = series;
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
    
    lane_v3 camera_position = V3(0, -10, 1);
    lane_v3 camera_z_axis = noz(camera_position);
    lane_v3 camera_x_axis = noz(cross(V3(0, 0, 1), camera_z_axis));
    lane_v3 camera_y_axis = noz(cross(camera_z_axis, camera_x_axis));
    lane_v3 film_center = camera_position - film_distance * camera_z_axis;
    
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
    state.camera_x_axis = extract_lane_0(camera_x_axis);
    state.camera_y_axis = extract_lane_0(camera_y_axis);
    state.camera_z_axis = extract_lane_0(camera_z_axis);
    state.camera_position = extract_lane_0(camera_position);
    state.half_film_width = 0.5f * film_width;
    state.half_film_height = 0.5f * film_height;
    state.film_center = extract_lane_0(film_center);
    state.half_pixel_width = 0.5f / buffer->width;
    state.half_pixel_height = 0.5f / buffer->height;
    state.bounces_computed = 0;
    state.loops_computed = 0;
    
    for(u32 row = y_min; row < y_count; ++row)
    {
        state.film_y = -1.0f + 2.0f * ((f32)row / (f32)buffer->height);
        for(u32 col = x_min; col < x_count; ++col)
        {
            state.film_x = -1.0f + 2.0f * ((f32)col / (f32)buffer->width);
            
            cast_sample_rays(&state);
            set_pixel(buffer, col, row, state.final_color);
            //bounces_computed += state.bounces_computed;
            //loops_computed += state.loops_computed;
        }
    }
    
    locked_add_u64(&queue->tiles_retired, 1);
    locked_add_u64(&queue->bounces_computed, state.bounces_computed);
    locked_add_u64(&queue->loops_computed, state.loops_computed);
    
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

internal void
display_buffer_in_window(render_state *rs)
{
    locked_exchange_u32(&rs->context_ready, 1);
    
    LARGE_INTEGER timer_start;
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&timer_start);
    
    while(rs->queue->tiles_retired < rs->total_tile_count)
    {
        printf("\rrendering... %.0f%%", ((f32)rs->queue->tiles_retired / (f32)rs->queue->work_order_count) * 100);
        fflush(stdout);
    }
    
    printf("\rrendering... %.0f%%", ((f32)rs->queue->tiles_retired / (f32)rs->queue->work_order_count) * 100);
    fflush(stdout);
    printf("\n");
    
    LARGE_INTEGER timer_end;
    QueryPerformanceCounter(&timer_end);
    timer_end.QuadPart = timer_end.QuadPart - timer_start.QuadPart;
    timer_end.QuadPart *= 1000000;
    timer_end.QuadPart /= frequency.QuadPart;
    f64 ms_elapsed = (f64)timer_end.QuadPart / (f64)1000;
    f64 ms_per_bounce = ms_elapsed / rs->queue->bounces_computed;
    printf("runtime: %.3Lf ms\n", ms_elapsed);
    printf("per-ray performance: %Lf ms\n", ms_per_bounce);
    locked_exchange_u32(&rs->render_in_progress, 0);
}
