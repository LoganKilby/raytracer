#include "types.h"
#include "macros.h"
#include "qpc.h"
#include "math_lib.h"
#include "main.h"

#include "ray.cpp"
#include "shading.cpp"
#include "scene_functions.cpp"

int main()
{
    f32 gamma = 1.0f;;
    pixel_buffer buffer;
    scene0(&buffer);
    write_ppm(buffer.data, buffer.width, buffer.height, gamma, "test.ppm");
    return 0;
}

inline void
set_pixel(pixel_buffer buffer, f32 x, f32 y, v4 color)
{
    if((x >= buffer.width || x < 0) || ((y >= buffer.height || y < 0)))
        return;
    
    v4 *pixel = (v4 *)&buffer.data[0] + (u32)y * buffer.width + (u32)x;
    *pixel = color;
}

internal void
clear(pixel_buffer buffer, v4 c)
{
    int pixel_count = buffer.width * buffer.height;
    for(v4 *pixel = (v4 *)&buffer.data[0]; 
        pixel < (v4 *)&buffer.data[0] + pixel_count; 
        ++pixel)
    {
        *pixel = c;
    }
}

internal pixel_buffer
new_pixel_buffer(u32 width, u32 height)
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
write_ppm(f32 *pixel_data, int width, int height, f32 gamma, char *filename)
{
    Assert(gamma);
    
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
    v4 pixel_color;
    
    if(gamma != 1.0)
    {
        f32 inv_gamma = 1.0f / gamma;
        for(f32 *row = pixel_data; 
            row < pixel_data + height * pitch; 
            row += pitch)
        {
            for(f32 *col = row; col < row + pitch; col += 4)
            {
                v3_f32_pow((v3 *)col, inv_gamma);
                r = (u8)(clampf(*(col), 0, 1) * 255);
                g = (u8)(clampf(*(col + 1), 0, 1) * 255);
                b = (u8)(clampf(*(col + 2), 0, 1) * 255);
                
                fprintf(file_handle, "%u %u %u ", r, g, b);
            }
            
            fprintf(file_handle, "\n");
        }
    }
    else
    {
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
    }
    
    fclose(file_handle);
    printf("%s\n", filename);
}
