#include "types.h"
#include "macros.h"
#include "math.h"
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
};

global_variable unsigned char global_pixel_data[1080 * 720 * 4];
internal void write_ppm(f32 *, int, int, char *);
internal pixel_buffer new_pixel_buffer(int width, int height);
internal void clear(pixel_buffer buffer, color4 color);

int main()
{
    mat4 m = new_mat4(-2, -8, 3, 5,
                      -3, 1, 7, 3,
                      1, 2, -9, 6,
                      -6, 7, 7, -9);
    f32 c00 = cofactor(m, 0, 0);
    f32 c01 = cofactor(m, 0, 1);
    f32 c02 = cofactor(m, 0, 2);
    f32 c03 = cofactor(m, 0, 3);
    f32 dm = det(m);
    
    Assert(c00 == 690);
    Assert(c01 == 447);
    Assert(c02 == 210);
    Assert(c03 == 51);
    Assert(dm == -4071);
    
    pixel_buffer buffer = new_pixel_buffer(1080, 720);
    clear(buffer, {0.35f, 0.70f, 1.0f, 1.0f});
    write_ppm(buffer.data, buffer.width, buffer.height, "test.ppm");
    return(0);
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
