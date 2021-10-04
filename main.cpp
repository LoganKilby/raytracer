#include "types.h"
#include "macros.h"
#include "math_functions.h"
#include "qpc.h"

struct projectile
{
    v4 position;
    v4 velocity;
};

struct environment
{
    v4 gravity;
    v4 wind_direction;
};

struct color4
{
    float r;
    float g;
    float b;
    float a;
};

struct pixel_buffer
{
    // NOTE: 4 bytes per pixel
    f32 *data;
    int width;
    int height;
    int pitch;
};

internal void write_ppm(f32 *, int, int, char *);
internal pixel_buffer new_pixel_buffer(int width, int height);
internal void clear(pixel_buffer buffer, color4 color);
internal void set_pixel(pixel_buffer buffer, int x, int y, color4 color);

int main()
{
    pixel_buffer buffer = new_pixel_buffer(1080, 720);
    
    mat4 ortho = ortho_matrix(0, 720, 0, 1080, 0, 1);
    ortho = mat4_transpose(ortho);
    v4 position = point(-1, -1, 0);
    //u32 x = min(buffer.width - 1, (ndc.x + 1) * 0.5 * buffer.width);
    //u32 y = min(buffer.height - 1, (ndc.y + 1) * 0.5 * buffer.height);
    
    clear(buffer, {0.0f, 0.0f, 0.0f, 1.0f});
    
    v4 ndc;
    for(int row = 0; row < 720; ++row)
    {
        ndc = v4_mat4_multiply(position, ortho);
        ndc.x /= ndc.z;
        ndc.y /= ndc.z;
        u32 x = (ndc.x * 0.5 * buffer.width) + 0.5f;
        u32 y = (ndc.y * 0.5 * buffer.height) + 0.5f;
        set_pixel(buffer, x, y, {1.0f, 1.0f, 1.0f, 1.0f});
        position.y++;
    }
    
    write_ppm(buffer.data, buffer.width, buffer.height, "test.ppm");
    return(0);
}

internal void
set_pixel(pixel_buffer buffer, int x, int y, color4 color)
{
    if((x >= buffer.width || x < 0) || (y >= buffer.height || y < 0))
        return;
    
    f32 *row = &buffer.data[0];
    f32 *pixel = row + buffer.pitch * y + x * 4;
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
    for(f32 *row = pixel_data; 
        row < pixel_data + height * pitch; 
        row += pitch)
    {
        u8 r, g, b;
        
        int chars_printed = 0;
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
