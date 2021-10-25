#include "types.h"
#include "macros.h"
#include "math_lib.h"
#include "main.h"
#include "time.h"
#include "include/GL/glew.h"
#include "include/GLFW/glfw3.h"

#include "ray.cpp"
#include "opengl.cpp"
#include "scene_functions.cpp"

// Forman Action (books):
// Numerical Methods That Work; Real Computing Made Real

// Paper on floating point arithmetic:
// https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html

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

int main()
{
    if(glfwInit())
    {
        printf("GLFW Initialized\n");
        printf(glfwGetVersionString());
        printf("\n\n");
    }
    else
    {
        // TODO: Logging
        printf("ERROR: GLFW failed to initialize\n");
    }
    
    s32 image_width = 1280;
    s32 image_height = 720;
    
#if 0
    GLFWwindow *Window = glfwCreateWindow(image_width, image_height, "raytracer", NULL, NULL);
    glfwMakeContextCurrent(Window);
    
    
    GLenum GlewError = glewInit();
    if(GlewError == GLEW_OK)
    {
        printf("INFO: OpenGL Initialized.\n");
        printf("Vendor: "); printf((char *)glGetString(GL_VENDOR)); printf("\n");
        printf("Renderer: "); printf((char *)glGetString(GL_RENDERER)); printf("\n");
        printf("OpenGL Version: "); printf((char *)glGetString(GL_VERSION)); printf("\n\n");
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        printf("ERROR: Glew failed to initialize");
    }
#endif
    
    pixel_buffer buffer = allocate_pixel_buffer(image_width, image_height);
    scene2_hmh(&buffer);
    
    f32 gamma = 2.2f;
    write_ppm(buffer.data, buffer.width, buffer.height, gamma, "test.ppm");
    return 0;
}

inline void
set_pixel(pixel_buffer *buffer, u32 col, u32 row, v4 color)
{
    if((col >= buffer->width || col < 0) || ((row >= buffer->height || row < 0)))
        return;
    
    v4 *pixel = (v4 *)&buffer->data[buffer->width * buffer->height * 4] - (row * buffer->width) - (buffer->width - col);
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
allocate_pixel_buffer(u32 width, u32 height)
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
    v3 *pixel;
    
    f32 inv_gamma = 1.0f / gamma;
    for(f32 *row = pixel_data; 
        row < pixel_data + height * pitch; 
        row += pitch)
    {
        for(f32 *col = row; col < row + pitch; col += 4)
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
    printf("%s\n", filename);
}
