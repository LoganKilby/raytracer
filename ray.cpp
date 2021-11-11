#include "ray.h"

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

// TODO: Test multiple triangles?
internal void
pbrt_triangle_intersection(triangle *tri, lane_v3 ray_origin, lane_v3 ray_direction, lane_f32 min_hit_distance, lane_f32 *hit_distance)
{
    // transform triangle verts to ray cooridnate space:
    // translate
    lane_v3 p0t = lane_v3_from_v3(tri->v0) - ray_origin;
    lane_v3 p1t = lane_v3_from_v3(tri->v1) - ray_origin;
    lane_v3 p2t = lane_v3_from_v3(tri->v2) - ray_origin;
    
    // permute components of triangle vertices and ray direction ?
    lane_u32 kz = max_dimensions(wabs(ray_direction));
    lane_u32 kx = kz + lane_u32_from_u32(1);
    conditional_assign(&kx, kx == lane_u32_from_u32(3), lane_u32_from_u32(0));
    lane_u32 ky = kx + lane_u32_from_u32(1);
    conditional_assign(&ky, ky == lane_u32_from_u32(3), lane_u32_from_u32(0));
    
    lane_v3 d = permute(ray_direction, kx, ky, kz);
    p0t = permute(p0t, kx, ky, kz);
    p1t = permute(p1t, kx, ky, kz);
    p2t = permute(p2t, kx, ky, kz);
    
    // apply shear transformations
    lane_f32 sz = 1.0f / d.z;
    lane_f32 sx = -d.x * sx;
    lane_f32 sy = -d.y * sx;
    
    p0t.x += sx * p0t.z;
    p0t.y += sy * p0t.z;
    p1t.x += sx * p1t.z;
    p1t.y += sy * p1t.z;
    p2t.x += sx * p2t.z;
    p2t.y += sy * p2t.z;
    
    // compute edge function coefficients e0, e1, and e2
    lane_f32 e0 = p1t.x * p2t.y - p1t.y * p2t.x;
    lane_f32 e1 = p2t.x * p0t.y - p2t.y * p0t.x;
    lane_f32 e2 = p0t.x * p1t.y - p0t.y * p1t.x;
    
    lane_u32 edge_mask = (e0 == zero_lane_f32) & (e1 == zero_lane_f32) & (e2 == zero_lane_f32);
    if(!mask_is_zeroed(edge_mask))
    {
        // can recompute with double precision for rays that fail the edge mask
        // and conditionally assign new edges
        lane_dv3 d_p0t = cast_lane_dv3(p0t);
        lane_dv3 d_p1t = cast_lane_dv3(p1t);
        lane_dv3 d_p2t = cast_lane_dv3(p2t);
        
        lane_f32 te0 = cast_lane_f32((d_p2t.x * d_p1t.y) - (d_p2t.y * d_p1t.x));
        lane_f32 te1 = cast_lane_f32((d_p0t.x * d_p2t.y) - (d_p0t.y * d_p2t.x));
        lane_f32 te2 = cast_lane_f32((d_p1t.x * d_p0t.y) - (d_p1t.y * d_p0t.x));
        
        conditional_assign(&e0, edge_mask, te0);
        conditional_assign(&e1, edge_mask, te1);
        conditional_assign(&e2, edge_mask, te2);
    }
    
    // triangle edge and determinant tests
    lane_u32 triangle_edge_mask = ((e0 < 0) | (e1 < 0) | (e2 < 0)) & ((e0 > 0) | (e1 > 0) | (e2 > 0));
    lane_f32 determinant = e0 + e1 + e2;
    lane_u32 det_mask = (determinant < EPSILON);
    
    if(!mask_is_zeroed(det_mask & triangle_edge_mask))
    {
        // compute scaled hit distance to triangle and test against ray_t range
        p0t.z *= sz;
        p1t.z *= sz;
        p1t.z *= sz;
        
        lane_f32 t_scaled = e0 * p0t.z + e1 * p1t.z + e2 * p2t.z;
        lane_u32 t_mask = (t_scaled <= zero_lane_f32) | (t_scaled > *hit_distance * determinant);
        lane_u32 hit_mask = (determinant < zero_lane_f32);
        
        if(!mask_is_zeroed(hit_mask & t_mask))
        {
            // compute barycentric coordinates and t value for triangle intersection
            lane_f32 inv_det = 1.0f / determinant;
            lane_f32 t = t_scaled * inv_det;
            
            // TODO: Implement
            
            /*
            lane_f32 max_zt = max_component(wabs(LaneV3(p0t.z, p1t.z, p2t.z)));
            lane_f32 g3 = gamma(3); // TODO: make constant
            lane_f32 delta_z = g3 * max_zt;
            */
            
            // compute delta_x, delta_y terms for triangle t error bounds
            /*
            lane_f32 max_xt = max_component(abs(LaneV3(p0t.x, p1t.x, p2t.x)));
            lane_f32 max_ty = max_component(abs(LaneV3(p0t.y, p1t.y, p2t.y)));
            
            lane_f32 g5 = gamma(5); // TODO: make constant
            lane_f32 g2 = gamma(2); // TODO: make constant
            lane_f32 delta_x = g5 * (max_xt + max_zt);
            lane_f32 delta_y = g5 * (max_yt + max_zt);
            lane_f32 delta_e = 2 * (g2 * max_xt * max_ty + delta_y * max_xt + delta_x * max_yt);
            lane_f32 max_e = max_component(wabs(LaneV3(e0, e1, e2)));
            lane_f32 delta_t = 3 * (g3 * max_e * max_zt + delta_e * max_zt + delta_z * max_e) * wabs(inv_det);
            
            lane_u32 hit_mask = (t <= delta_t);
conditonally_assign(hit_distance, hit_mask, t);
conditonally_assign(hit_normal, hit_mask, normal);
conditionall_assign(hit_mat_index, hit_mask, mat_index);
*/
        }
    }
}

internal void
eberly_triangle_intersection(triangle *tri, lane_v3 ray_direction, lane_v3 ray_origin)
{
    lane_f32 min_hit_distance = lane_f32_from_f32(0);
    lane_f32 hit_distance;
    lane_u32 hit_mat_index;
    lane_v3 hit_normal;
    
    //////////
    
    lane_v3 vert0 = lane_v3_from_v3(tri->v0);
    lane_v3 vert1 = lane_v3_from_v3(tri->v1);
    lane_v3 vert2 = lane_v3_from_v3(tri->v2);
    lane_v3 normal = lane_v3_from_v3(tri->normal);
    
    // NOTE: Does not cull back-facing triangles
    lane_v3 e1, e2, p, s, q;
    lane_f32 t, u, v, tmp;
    
    e1 = vert1 - vert0;
    e2 = vert2 - vert0;
    p = cross(ray_direction, e2);
    tmp = inner(p, e1);
    
    lane_u32 u32_max = lane_u32_from_u32(0xFFFFFFFF);
    lane_u32 tmp_mask = ((tmp > -EPSILON) & (tmp < EPSILON)) ^ u32_max;
    if(!mask_is_zeroed(tmp_mask))
    {
        tmp = 1.0f / tmp;
        s = ray_origin - vert0;
        u = tmp * inner(s, p);
        
        lane_u32 u_mask = ((u < 0) | (u > 1.0f)) ^ u32_max;
        if(!mask_is_zeroed(u_mask))
        {
            q = cross(s, e1);
            v = tmp * inner(ray_direction, q);
            
            lane_u32 v_mask = (v < 0) | (v > 1);
            lane_u32 uv_mask = ((u + v) > 1);
            if(!mask_is_zeroed((v_mask & uv_mask) ^ u32_max))
            {
                t = tmp * inner(e2, q);
                lane_u32 t_mask = (t > min_hit_distance) & (t < hit_distance);
                conditional_assign(&hit_distance, t_mask, t);
                conditional_assign(&hit_mat_index, t_mask, lane_u32_from_u32(5));
                conditional_assign(&hit_normal, t_mask, normal);
                //lane_v3 intersection = ray_origin + t * ray_direction;
            }
        }
    }
    
    lane_v3 intersection = ray_origin + t * ray_direction;
}

internal void
scratch_triangle_intersection(triangle tri)
{
    lane_v3 ray_direction = lane_v3_from_v3(V3(0, 0, 0));
    lane_v3 hit_normal = lane_v3_from_v3(V3(0, 0, 0));
    lane_v3 ray_origin = lane_v3_from_v3(V3(0, 0, 0));
    lane_f32 hit_distance = lane_f32_from_f32(0);
    lane_u32 hit_mat_index;
    
    lane_v3 v0 = lane_v3_from_v3(tri.v0);
    lane_v3 v1 = lane_v3_from_v3(tri.v1);
    lane_v3 v2 = lane_v3_from_v3(tri.v2);
    
    lane_v3 v0v1 = v1 - v0;
    lane_v3 v0v2 = v2 - v0;
    lane_v3 normal = lane_v3_from_v3(tri.normal);
    
    // Should equal stored normal
    lane_v3 test = cross(v0v1, v0v2);
    
    lane_f32 area2 = length(normal);
    
    // check if ray and plane are parallel
    lane_f32 ndr = inner(normal, ray_direction);
    //lane_u32 abs_mask = wabs(ndr) > EPSILON;
    
    //if(!mask_is_zeroed(abs_mask))
    {
        lane_f32 d = inner(normal, v0);
        lane_f32 t = (inner(normal, ray_origin) + d) / ndr;
        lane_u32 t_mask = (t > EPSILON) & (t < hit_distance);
        
        if(!mask_is_zeroed(t_mask))
        {
            lane_v3 p = ray_origin + t * ray_direction;
            lane_v3 c; // vector perp to triangle's plane
            lane_v3 edge;
            lane_v3 vp;
            
            lane_f32 zero_lane = lane_f32_from_f32(0);
            
            edge = v1 - v0;
            vp = p - v0;
            c = cross(edge, vp);
            lane_u32 edge0_mask = (inner(normal, c) >= zero_lane);
            
            edge = v2 - v1;
            vp = p - v1;
            c = cross(edge, vp);
            lane_u32 edge1_mask = (inner(normal, c) >= zero_lane);
            
            edge = v0 - v2;
            vp = p - v2;
            c = cross(edge, vp);
            lane_u32 edge2_mask = (inner(normal, c) >= zero_lane);
            
            lane_u32 hit_mask = edge0_mask & edge1_mask & edge2_mask;
            conditional_assign(&hit_distance, hit_mask, t);
            conditional_assign(&hit_mat_index, hit_mask, lane_u32_from_u32(5));
            conditional_assign(&hit_normal, hit_mask, normal);
        }
    }
}

internal void
cast_sample_rays(ray_cast_state *state)
{
    // in
    world *world = state->world; 
    triangle_buffer tri_buffer = state->tri_buffer;
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
    lane_u32 u32_max = lane_u32_from_u32(0xFFFFFFFF);
    
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
        lane_v3 attenuation = LaneV3(1, 1, 1);
        
        lane_u32 lane_mask = lane_u32_from_u32(0xFFFFFFFF);
        for(u32 bounce_count = 0; bounce_count < max_bounce_count; ++bounce_count)
        {
            lane_v3 hit_normal = {};
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
                    }
                }
            }
            
            for(u32 tri_index = 0; tri_index < tri_buffer.count; ++tri_index)
            {
                lane_v3 v0 = lane_v3_from_v3(tri_buffer.tri[tri_index].v0);
                lane_v3 v1 = lane_v3_from_v3(tri_buffer.tri[tri_index].v1);
                lane_v3 v2 = lane_v3_from_v3(tri_buffer.tri[tri_index].v2);
                lane_v3 normal = lane_v3_from_v3(tri_buffer.tri[tri_index].normal);
                
#if 0
                lane_v3 v0v1 = v1 - v0;
                lane_v3 v0v2 = v2 - v0;
                lane_v3 pv = cross(ray_direction, v0v2);
                lane_f32 det = inner(v0v1, pv);
                
                lane_u32 det_mask = (det < -min_hit_distance) | (det > min_hit_distance);
                
                //det_mask &= (det > min_hit_distance);
                
                if(!mask_is_zeroed(det_mask))
                {
                    lane_f32 inv_det = 1 / det;
                    lane_v3 tv = ray_origin - v0;
                    lane_f32 u = inner(tv, pv) * inv_det;
                    lane_u32 u_mask = ((u >= 0) & (u <= 1));
                    if(!mask_is_zeroed(u_mask))
                    {
                        lane_v3 qv = cross(tv, v0v1);
                        lane_f32 v = inner(ray_direction, qv) * inv_det;
                        lane_u32 v_mask = ((v >= 0) & (v <= 1));
                        if(!mask_is_zeroed(v_mask))
                        {
                            lane_f32 t = inner(v0v2, qv) * inv_det;
                            lane_u32 t_mask = (t > min_hit_distance) & (t < hit_distance);
                            conditional_assign(&hit_distance, t_mask, t);
                            conditional_assign(&hit_mat_index, t_mask, lane_u32_from_u32(1));
                            conditional_assign(&hit_normal, t_mask, normal);
                        }
                    }
                }
                
#if 0
                // Should equal stored normal
                //lane_v3 normal = cross(v0v1, v0v2);
                
                //lane_f32 area2 = length(normal);
                
                // check if ray and plane are parallel
                lane_f32 denom = inner(normal, ray_direction);
                //lane_u32 abs_mask = wabs(denom) > EPSILON;
                lane_u32 denom_mask = (denom < -min_hit_distance) | (denom > min_hit_distance);
                
                if(!mask_is_zeroed(denom_mask))
                {
                    lane_f32 d = inner(normal, v0);
                    lane_f32 t = (inner(normal, ray_origin) + d) / denom;
                    lane_u32 t_mask = ((t > EPSILON) & (t < hit_distance));
                    
                    if(!mask_is_zeroed(t_mask))
                    {
                        lane_v3 p = ray_origin + t * ray_direction;
                        lane_v3 c; // vector perp to triangle's plane
                        lane_v3 edge;
                        lane_v3 vp;
                        
                        edge = v1 - v0;
                        vp = p - v0;
                        c = cross(edge, vp);
                        lane_u32 edge0_mask = (inner(normal, c) < 0) ^ u32_max;
                        
                        edge = v2 - v1;
                        vp = p - v1;
                        c = cross(edge, vp);
                        lane_u32 edge1_mask = (inner(normal, c) < 0) ^ u32_max;
                        
                        edge = v0 - v2;
                        vp = p - v2;
                        c = cross(edge, vp);
                        lane_u32 edge2_mask = (inner(normal, c) < 0) ^ u32_max;
                        
                        lane_u32 hit_mask = (edge0_mask & edge1_mask & edge2_mask);
                        
                        conditional_assign(&hit_distance, hit_mask, t);
                        conditional_assign(&hit_mat_index, hit_mask, lane_u32_from_u32(1));
                        conditional_assign(&hit_normal, hit_mask, normal);
                    }
                }
#endif
                
#else
                // NOTE: Does not cull back-facing triangles
                lane_v3 e1, e2, p, s, q;
                lane_f32 t, u, v, denom;
                
                e1 = v1 - v0;
                e2 = v2 - v0;
                p = cross(ray_direction, e2);
                denom = inner(p, e1);
                
                lane_u32 denom_mask = (denom > EPSILON);
                if(!mask_is_zeroed(denom_mask))
                {
                    denom = 1.0f / denom;
                    s = ray_origin - v0;
                    u = denom * inner(s, p);
                    
                    lane_u32 u_mask = ((u >= 0) & (u <= 1));
                    if(!mask_is_zeroed(u_mask))
                    {
                        q = cross(s, e1);
                        v = denom * inner(ray_direction, q);
                        
                        lane_u32 v_mask = (v >= 0) & (v <= 1);
                        lane_u32 uv_mask = ((u + v) <= 1);
                        if(!mask_is_zeroed((v_mask & uv_mask)))
                        {
                            t = denom * inner(e2, q);
                            lane_u32 t_mask = (t > min_hit_distance) & (t < hit_distance);
                            
                            conditional_assign(&hit_distance, t_mask, t);
                            conditional_assign(&hit_mat_index, t_mask, lane_u32_from_u32(1));
                            conditional_assign(&hit_normal, t_mask, normal);
                            //lane_v3 intersection = ray_origin + t * ray_direction;
                        }
                    }
                }
                
#endif
            }
            
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
    
    lane_v3 camera_position = LaneV3(0, -10, 1);
    lane_v3 camera_z_axis = noz(camera_position);
    lane_v3 camera_x_axis = noz(cross(LaneV3(0, 0, 1), camera_z_axis));
    lane_v3 camera_y_axis = noz(cross(camera_z_axis, camera_x_axis));
    lane_v3 film_center = camera_position - film_distance * camera_z_axis;
    
    work_order *order = queue->work_orders + work_order_index;
    u32 x_min = order->min_x;
    u32 y_min = order->min_y;
    u32 x_count = order->max_x;
    u32 y_count = order->max_y;
    pixel_buffer *screen_buffer = order->screen_buffer;
    
    if(screen_buffer->width > screen_buffer->height)
    {
        film_height = film_width * (f32)screen_buffer->height / (f32)screen_buffer->width;
    }
    else if(screen_buffer->height > screen_buffer->width)
    {
        film_width = film_height * (f32)screen_buffer->width / (f32)screen_buffer->height;
    }
    
    ray_cast_state state = {};
    state.rays_per_pixel = queue->rays_per_pixel;
    state.max_bounce_count = queue->max_bounce_count;
    state.world = order->world;
    state.tri_buffer = order->tri_buffer;
    state.series = &order->entropy;
    state.camera_x_axis = extract_lane_0(camera_x_axis);
    state.camera_y_axis = extract_lane_0(camera_y_axis);
    state.camera_z_axis = extract_lane_0(camera_z_axis);
    state.camera_position = extract_lane_0(camera_position);
    state.half_film_width = 0.5f * film_width;
    state.half_film_height = 0.5f * film_height;
    state.film_center = extract_lane_0(film_center);
    state.half_pixel_width = 0.5f / screen_buffer->width;
    state.half_pixel_height = 0.5f / screen_buffer->height;
    state.bounces_computed = 0;
    state.loops_computed = 0;
    
    for(u32 row = y_min; row < y_count; ++row)
    {
        state.film_y = -1.0f + 2.0f * ((f32)row / (f32)screen_buffer->height);
        for(u32 col = x_min; col < x_count; ++col)
        {
            state.film_x = -1.0f + 2.0f * ((f32)col / (f32)screen_buffer->width);
            
            cast_sample_rays(&state);
            set_pixel(screen_buffer, col, row, state.final_color);
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
