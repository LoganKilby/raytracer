#include "types.h"
#include "macros.h"
#include "qpc.h"
#include "math_lib.h"

#include "ray.cpp"
#include "shading.cpp"

struct pixel_buffer
{
    // NOTE: 4 floats per pixel
    f32 *data;
    int width;
    int height;
    int pitch;
};

internal void write_ppm(f32 *, int, int, char *);
internal pixel_buffer new_pixel_buffer(int width, int height);
internal void clear(pixel_buffer buffer, v4 color);
internal void set_pixel(pixel_buffer buffer, f32 x, f32 y, v4 color);

internal void
draw_scene(pixel_buffer buffer)
{
    sphere s = default_sphere();
    s.material.color = color(1, 0.2f, 1);
    
    point_light light;
    light.intensity = color(1, 1, 1);
    light.position = point(-10, 0, -5);
    
    f32 wall_z = 10.0f;
    f32 wall_size = 7.0f;
    f32 wall_half = wall_size / 2;
    f32 pixel_size = wall_size / buffer.width;
    
    f32 world_y;
    f32 world_x;
    v4 position;
    v4 point;
    v4 pixel_color;
    intersection intersect;
    ray r;
    r.origin = point(0, 0, -5.0f);
    
    for(int row = 0; row < buffer.height; ++row)
    {
        world_y = wall_half - pixel_size * row;
        for(int col = 0; col < buffer.width; ++col)
        {
            world_x = -wall_half + pixel_size * col;
            position = point(world_x, world_y, wall_z);
            r.direction = glm::normalize(position - r.origin);
            if(ray_sphere_intersection(r, s, &intersect))
            {
                f32 t = ray_hit(intersect.t0, intersect.t1);
                if(t)
                {
                    v4 point = ray_hit_position(r, t);
                    v4 surface_normal = sphere_normal(s.transform, point);
                    pixel_color = calc_point_light(s.material, light, point, -r.direction, -surface_normal);
                    set_pixel(buffer, col, row, pixel_color);
                }
            }
        }
    }
}

int main()
{
    pixel_buffer buffer = new_pixel_buffer(100, 100);
    draw_scene(buffer);
    write_ppm(buffer.data, buffer.width, buffer.height, "test.ppm");
    return(0);
}

internal void
set_pixel(pixel_buffer buffer, f32 x, f32 y, v4 pixel_color)
{
    if((x >= buffer.width || x < 0) || (y >= buffer.height || y < 0))
        return;
    
    f32 *row = &buffer.data[0];
    f32 *pixel = row + (buffer.pitch * (u32)y) + ((u32)x * 4);
    *(pixel)     = pixel_color.r;
    *(pixel + 1) = pixel_color.g;
    *(pixel + 2) = pixel_color.b;
}

internal void
clear(pixel_buffer buffer, v4 color)
{
    // 4 bytes per pixel
    int pitch = buffer.width * 4;
    for(f32 *row = buffer.data; 
        row < buffer.data + buffer.height * pitch; 
        row += pitch)
    {
        for(f32 *col = row; col < row + pitch; col += 4)
        {
            *(col)     = color.r;
            *(col + 1) = color.b;
            *(col + 2) = color.g;
            *(col + 3) = color.a;
        }
    }
}

internal pixel_buffer
new_pixel_buffer(int width, int height)
{
    pixel_buffer result;
    result.pitch = width * 4;
    int buffer_size = sizeof(f32) * width * height * 4;
    result.data = (f32 *)malloc(buffer_size);
    memset(result.data, 0, buffer_size);
    result.width = width;
    result.height = height;
    return result;
}

internal f32
clampf(f32 x, f32 min, f32 max)
{
    if(x >= min && x <= max)
    {
        return x;
    }
    else 
    {
        return x < min ? min : max;
    }
}

internal void
write_ppm(f32 *pixel_data, int width, int height, char *filename)
{
    FILE *file_handle = fopen(filename, "w");
    
    if(!file_handle)
    {
        printf("unable to write to file %s\n", filename);
        return;
    }
    
    // ppm header
    fprintf(file_handle, "P3\n%u %u\n255\n", width, height);
    
    int pitch = width * 4;
    u8 r, g, b;
    for(f32 *row = pixel_data; 
        row < pixel_data + height * pitch; 
        row += pitch)
    {
        for(f32 *col = row; col < row + pitch; col += 4)
        {
            r = (u8)(clampf(*(col), 0, 1) * 255);
            g = (u8)(clampf(*(col + 1), 0, 1) * 255);
            b = (u8)(clampf(*(col + 2), 0, 1) * 255);
            
            fprintf(file_handle, "%u %u %u ", r, g, b);
        }
        
        fprintf(file_handle, "\n");
    }
    
    fclose(file_handle);
    printf("%s\n", filename);
}
