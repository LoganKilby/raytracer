#include "types.h"
#include "macros.h"
#include "math_lib.h"
#include "main.h"
#include "time.h"
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

#include "ray.cpp"
#include "preview.cpp"

// Forman Action (books):
// Numerical Methods That Work -- Real Computing Made Real

// Paper on floating point arithmetic:
// https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html

// TODO: Write own acos, atan2 functions

internal f32
linear_to_srgb(f32 linear)
{
    if(linear < 0.0f)
    {
        linear = 0.0f;
    }
    
    if(linear > 1.0f)
    {
        linear = 1.0f;
    }
    
    f32 srgb = linear * 12.95f;;
    if(linear > 0.0031308f)
    {
        srgb = 1.055f * (f32)pow(linear, 1.0f/2.4f) - 0.055f;
    }
    
    return srgb;
}

global_variable b32 render_progress_to_window;

int main(int argc, char **argv)
{
    bool show_preview = true;
    bool multithreaded = true;
    
    s32 image_width = 1280;
    s32 image_height = 720;
    pixel_buffer buffer = allocate_pixel_buffer(image_width, image_height);
    
    material materials[7] = {};
    materials[0].emit_color = {0.3f, 0.4f, 0.5f}; // sky
    materials[1].reflect_color = {0.5f, 0.5f, 0.5f}; // ground
    materials[2].reflect_color = {0.7f, 0.5f, 0.3f};
    materials[3].emit_color = {5.0f, 0.0f, 0.0f};
    materials[4].reflect_color = {0.2f, 0.8f, 0.2f};
    materials[5].reflect_color = {0.4f, 0.8f, 0.9f};
    materials[5].specular = 0.85f;
    materials[6].reflect_color = {0.95f, 0.95f, 0.95f};
    materials[6].specular = 1.0f;
    
    plane planes[1] = {};
    planes[0].d = 0;
    planes[0].normal = {0, 0, 1};
    planes[0].material_index = 1;
    
    sphere spheres[5] = {};
    spheres[0].origin = {0, 0, 0};
    spheres[0].radius = 1;
    spheres[0].material_index = 2;
    spheres[1].origin = {3, -2, 0};
    spheres[1].radius = 1;
    spheres[1].material_index = 3;
    spheres[2].origin = {-2, -1, 2};
    spheres[2].radius = 1.0;
    spheres[2].material_index = 4;
    spheres[3].origin = {1, -1, 3};
    spheres[3].radius = 1.0;
    spheres[3].material_index = 5;
    spheres[4].origin = {2.5, 3, 0};
    spheres[4].radius = 2.0;
    spheres[4].material_index = 6;
    
    world world = {};
    world.material_count = array_count(materials);
    world.materials = materials;
    world.plane_count = array_count(planes);
    world.planes = planes;
    world.sphere_count = array_count(spheres);
    world.spheres = spheres;
    
    u32 core_count = multithreaded ? get_core_count() : 1;
    u32 tile_width = buffer.width / core_count;
    u32 tile_height = tile_width;
    u32 tile_count_x = (buffer.width + tile_width - 1) / tile_width;
    u32 tile_count_y = (buffer.height + tile_height - 1) / tile_height;
    u32 total_tile_count = tile_count_x * tile_count_y;
    printf("processing: %d cores and %d %dx%d (%dk/tile) tiles, %d-wide lanes\n", core_count, total_tile_count, tile_width, tile_height, tile_width * tile_height * 4 * 4 / 1024, LANE_WIDTH);
    
    work_queue queue = {};
    queue.rays_per_pixel = 256;
    
    if(argc == 2)
    {
        queue.rays_per_pixel = atoi(argv[1]);
    }
    
    queue.max_bounce_count = 8;
    queue.work_orders = (work_order *)malloc(sizeof(work_order) * total_tile_count);
    printf("resolution: %d by %d pixels. %d rays per pixel. %d maximum bounces per pixel\n", buffer.width, buffer.height, queue.rays_per_pixel, queue.max_bounce_count);
    
    for(u32 tile_y = 0; tile_y < tile_count_y; ++tile_y)
    {
        u32 min_y = tile_y * tile_height;
        u32 max_y = min_y + tile_height;
        if(max_y > buffer.height)
        {
            max_y = buffer.height;
        }
        
        for(u32 tile_x = 0; tile_x < tile_count_x; ++tile_x)
        {
            u32 min_x = tile_x * tile_width;
            u32 max_x = min_x + tile_width;
            if(max_x > buffer.width)
            {
                max_x = buffer.width;
            }
            
            work_order *order = queue.work_orders + queue.work_order_count++;
            Assert(queue.work_order_count <= total_tile_count);
            
            order->world = &world;
            order->buffer = &buffer; 
            order->min_x = min_x;
            order->min_y = min_y;
            order->max_x = max_x;
            order->max_y = max_y;
            
            // TODO: repleace with real entropy
            random_series entropy = 
            {
                lane_u32_from_u32((2334598 + tile_x * 5535 + tile_y * 64568),
                                  (7444598 + tile_x * 5675 + tile_y * 32488),
                                  (4097254 + tile_x * 1235 + tile_y * 62314),
                                  (234098 + tile_x * 9905 + tile_y * 73688),
                                  (987876 + tile_x * 5765456 + tile_y * 790234),
                                  (678900 + tile_x * 980986 + tile_y * 34232),
                                  (304230 + tile_x * 7095648 + tile_y * 3057790),
                                  (866345 + tile_x * 73498 + tile_y * 734345),
                                  (239823 + tile_x * 490872 + tile_y * 350972),
                                  (3509898 + tile_x * 756253 + tile_y * 565656),
                                  (5020983 + tile_x * 4239487 + tile_y * 635235),
                                  (6623423 + tile_x * 4918274 + tile_y * 655373),
                                  (8292847 + tile_x * 58213 + tile_y * 40987),
                                  (202348 + tile_x * 5485550 + tile_y * 030332),
                                  (3485992 + tile_x * 3423488 + tile_y * 800902),
                                  (849320 + tile_x * 572048 + tile_y * 5023937)),
            };
            
            order->entropy = entropy;
        }
    }
    
    Assert(queue.work_order_count == total_tile_count);
    
    // NOTE: fencing, probably not neccessary
    locked_add_u64(&queue.next_work_order_index, 0);
    
    LARGE_INTEGER timer_start;
    LARGE_INTEGER frequency;
    
    if(show_preview)
    {
        preview_context preview = setup_preview_window(image_width, image_height);
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timer_start);
        for(u32 core_index = 1; core_index < core_count; ++core_index)
        {
            create_worker_thread(&queue);
        }
        
        while(queue.tiles_retired < total_tile_count)
        {
            
            printf("\rrendering... %.0f%%", ((f32)queue.tiles_retired / (f32)queue.work_order_count) * 100);
            fflush(stdout);
            update_preview(&preview, buffer.data);
        }
        
        update_preview(&preview, buffer.data);
    }
    else
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timer_start);
        for(u32 core_index = 1; core_index < core_count; ++core_index)
        {
            create_worker_thread(&queue);
        }
        
        while(render_tile(&queue) || queue.tiles_retired < total_tile_count)
        {
            printf("\rrendering... %.0f%%", ((f32)queue.tiles_retired / (f32)queue.work_order_count) * 100);
            fflush(stdout);
        }
    }
    
    printf("\rrendering... %.0f%%", ((f32)queue.tiles_retired / (f32)queue.work_order_count) * 100);
    fflush(stdout);
    printf("\n");
    
    LARGE_INTEGER timer_end;
    QueryPerformanceCounter(&timer_end);
    timer_end.QuadPart = timer_end.QuadPart - timer_start.QuadPart;
    timer_end.QuadPart *= 1000000;
    timer_end.QuadPart /= frequency.QuadPart;
    f64 ms_elapsed = (f64)timer_end.QuadPart / (f64)1000;
    f64 ms_per_bounce = ms_elapsed / queue.bounces_computed;
    u64 total_bounces = queue.loops_computed;
    u64 used_bounces = queue.bounces_computed;
    u64 wasted_bounces = total_bounces - used_bounces;
    printf("runtime: %.3Lf ms\n", ms_elapsed);
    printf("per-ray performance: %Lf ms\n", ms_per_bounce);
    printf("total bounces: %llu\n", total_bounces);
    printf("used bounces: %llu\n", used_bounces);
    printf("wasted bounces: %llu (%.02f%%)\n", wasted_bounces, 100.0f * (f32)wasted_bounces / (f32)total_bounces);
    
    f32 gamma = 2.2f;
    write_ppm(buffer.data, buffer.width, buffer.height, gamma, "test.ppm");
    
    return 0;
}

internal void
set_pixel(pixel_buffer *buffer, u32 col, u32 row, v3 color)
{
    if((col >= buffer->width || col < 0) || ((row >= buffer->height || row < 0)))
        return;
    
    v3 *pixel = (v3 *)(buffer->data + row * buffer->width * 3 + col * 3);
    *pixel = color;
}

internal void
clear(pixel_buffer buffer, v3 c)
{
    int pixel_count = buffer.width * buffer.height;
    for(v3 *pixel = (v3 *)&buffer.data[0]; 
        pixel < (v3 *)&buffer.data[0] + pixel_count; 
        ++pixel)
    {
        *pixel = c;
    }
}

internal pixel_buffer
allocate_pixel_buffer(u32 width, u32 height)
{
    pixel_buffer result;
    result.pitch = width * 3;
    int buffer_size = sizeof(f32) * width * height * 3;
    result.data = (f32 *)malloc(buffer_size);
    memset(result.data, 0, buffer_size);
    result.width = width;
    result.height = height;
    return result;
}

internal void
write_ppm(f32 *pixel_data, int width, int height, f32 gamma, char *file_name)
{
    Assert(gamma);
    
    FILE *file_handle = fopen(file_name, "w");
    
    if(!file_handle)
    {
        printf("unable to write to file %s\n", file_name);
        return;
    }
    
    // ppm header
    fprintf(file_handle, "P3\n%u %u\n255\n", width, height);
    
    int pitch = width * 3;
    u8 r, g, b;
    v3 *pixel;
    
    f32 inv_gamma = 1.0f / gamma;
    for(f32 *row = pixel_data; 
        row < pixel_data + height * pitch; 
        row += pitch)
    {
        for(f32 *col = row; col < row + pitch; col += 3)
        {
            pixel = (v3 *)col;
            r = (u8)(linear_to_srgb(pixel->r) * 255);
            g = (u8)(linear_to_srgb(pixel->g) * 255);
            b = (u8)(linear_to_srgb(pixel->b) * 255);
            
            fprintf(file_handle, "%u %u %u ", r, g, b);
        }
        
        fprintf(file_handle, "\n");
    }
    
    fclose(file_handle);
    
    char dir[256] = {};
    GetCurrentDirectory(sizeof(dir), dir);
    printf("out: %s\\%s\n", dir, file_name);
}
