#include "types.h"
#include "macros.h"
#include "math_functions.h"
#include "qpc.h"
#include "ray.cpp"

struct color4
{
    float r;
    float g;
    float b;
    float a;
};

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
internal void clear(pixel_buffer buffer, color4 color);
internal void set_pixel(pixel_buffer buffer, f32 x, f32 y, color4 color);
internal u2 screen_coordinates(v4 position, mat4 ortho, s32 width, s32 height);

internal void
cast_sphere_shadow(pixel_buffer buffer)
{
    clear(buffer, {0.0f, 0.0f, 0.0f, 1.0f});
    
    sphere s = new_sphere({0, 0, 0}, 1);
    s.transform = mat4_shear(1, 0, 0, 1, 0, 0);
    s.transform = mat4_scale(s.transform, {0.5, 1, 1});
    
    ray r;
    v4 ray_origin = point(0, 0, -5);
    
    f32 wall_z = 10;
    f32 wall_size = 7.0;
    f32 pixel_size = wall_size / 100;
    f32 half_wall = wall_size / 2;
    
    f32 world_x;
    f32 world_y;
    
    intersection i;
    
    int quarter_row = buffer.height / 4;
    printf("drawing scene\n");
    for(int row = 0; row < buffer.height; ++row)
    {
        if(row == quarter_row)
        {
            printf("25%%...\n");
        }
        else if(row == quarter_row * 2)
        {
            printf("50%%...\n");
        }
        else if(row == quarter_row * 3)
        {
            printf("75%%...\n");
        }
        
        world_y = half_wall - (pixel_size * row);
        for(int col = 0; col < buffer.width; ++col)
        {
            world_x = -half_wall + pixel_size * col;
            v4 position = point(world_x, world_y, wall_z);
            r = new_ray(ray_origin, v4_normalize(v4_sub(position, ray_origin)));
            ray_sphere_intersection(r, s, &i);
            f32 hit = ray_hit(i.t0, i.t1);
            if(hit > 0)
            {
                set_pixel(buffer, col, row, {1.0f, 1.0f, 1.0f, 1.0f});
            }
            
            
        }
    }
    
    printf("finished.\n");
}

int main()
{
    pixel_buffer buffer = new_pixel_buffer(100, 100);
    printf("buffer created\n");
    cast_sphere_shadow(buffer);
    write_ppm(buffer.data, buffer.width, buffer.height, "test.ppm");
    return(0);
}

internal u2
screen_coordinates(v4 position, mat4 ortho, s32 width, s32 height)
{
    v4 ndc = v4_mat4_multiply(position, ortho);
    ndc.x /= ndc.z;
    ndc.y /= ndc.z;
    u2 screen_coords;
    screen_coords.x = ((ndc.x + 1) * 0.5 * width) + 0.5f;
    screen_coords.y = ((ndc.y + 1) * 0.5 * height) + 0.5f;
    return(screen_coords);
}

internal void
set_pixel(pixel_buffer buffer, f32 x, f32 y, color4 color)
{
    if((x >= buffer.width || x < 0) || (y >= buffer.height || y < 0))
        return;
    
    f32 *row = &buffer.data[0];
    f32 *pixel = row + (buffer.pitch * (u32)y) + ((u32)x * 4);
    *(pixel)     = color.r;
    *(pixel + 1) = color.g;
    *(pixel + 2) = color.b;
}

internal void
clear(pixel_buffer buffer, color4 color)
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
            r = (u8)clampf(*(col), 0, 1) * 255;
            g = (u8)clampf(*(col + 1), 0, 1) * 255;
            b = (u8)clampf(*(col + 2), 0, 1) * 255;
            
            fprintf(file_handle, "%u %u %u ", r, g, b);
        }
        
        fprintf(file_handle, "\n");
    }
    
    printf("%s\n", filename);
}
