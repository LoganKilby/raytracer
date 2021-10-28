#include "preview.h"

internal u32
link_program(GLuint vs_id, GLuint fs_id)
{
    u32 result = glCreateProgram();
    
    glAttachShader(result, vs_id);
    glAttachShader(result, fs_id);
    glLinkProgram(result);
    
    int link_status;
    glGetProgramiv(result, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE)
    {
        fprintf(stderr, "ERROR: Shader link error (Program ID: %d)\n", result);
        int msg_length;
        glGetProgramiv(result, GL_INFO_LOG_LENGTH, &msg_length);
        if(msg_length)
        {
            char *msg_buffer = (char *)malloc(msg_length);
            memset(msg_buffer, 0, msg_length);
            glGetProgramInfoLog(result, msg_length, NULL, msg_buffer);
            fprintf(stderr, "%s\n\n", msg_buffer);
            free(msg_buffer);
        }
    }
    
    // NOTE: glDeleteShader sets a flag for deletion. A shader won't be deleted while attached to a program.
    glDeleteShader(vs_id); 
    glDeleteShader(fs_id);
    
    return result;
}

static char *
os_read_entire_file(char *file_name)
{
    // TODO: I should really just use fread
    
    HANDLE file_handle = CreateFileA(file_name,
                                     GENERIC_READ,
                                     0,
                                     0,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL, // TODO: Read only?
                                     0);
    
    if(file_handle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "WARNING: Windows could not open the file %s.\n", file_name);
        return 0;
    }
    
    u32 file_size = GetFileSize(file_handle, 0);
    char *file_buffer = 0;
    if(file_size)
    {
        file_buffer = (char *)malloc(file_size + 1);
        memset(file_buffer, 0, file_size + 1);
        
        unsigned long bytes_read = 0;
        ;if(!ReadFile(file_handle, file_buffer, file_size, &bytes_read, 0))
        {
            fprintf(stderr, "WARNING: Windows encountered an error while reading from \"%s\". File read aborted.", file_name);
            free(file_buffer);
            file_buffer = 0;
        }
    }
    
    CloseHandle(file_handle);
    
    return file_buffer;
}

internal u32
compile_shader(char *shader_source, GLenum shader_type)
{
    u32 result = 0;
    char *shader_type_string;
    switch(shader_type)
    {
        case GL_VERTEX_SHADER:
        shader_type_string = "GL_VERTEX_SHADER";
        break;
        case GL_FRAGMENT_SHADER:
        shader_type_string= "GL_FRAGMENT_SHADER";
        break;
        default:
        {
            shader_type_string = "UNKNOWN_SHADER_TYPE";
        }
    }
    
    if(shader_source)
    {
        result = glCreateShader(shader_type);
        glShaderSource(result, 1, &shader_source, NULL);
        glCompileShader(result);
        
        int compilation_status;
        glGetShaderiv(result, GL_COMPILE_STATUS, &compilation_status);
        if(compilation_status == GL_FALSE)
        {
            int log_length;
            glGetShaderiv(result, GL_INFO_LOG_LENGTH, &log_length);
            char *msg_buffer = (char *)malloc(log_length);
            memset(msg_buffer, 0, log_length);
            glGetShaderInfoLog(result, log_length, NULL, msg_buffer);
            
            fprintf(stderr, "\nERROR: Shader compilation error (%s):\n", shader_type_string);
            fprintf(stderr, "%s", msg_buffer);
            fprintf(stderr, "\n%s\n\n", shader_source);
            
            free(msg_buffer);
        }
    }
    
    return result;
}

internal u32
load_compile_shader(char *file_name, GLenum shader_type)
{
    char *shader_source = os_read_entire_file(file_name);
    
    u32 result = 0;
    if(shader_source)
    {
        result = compile_shader(shader_source, shader_type);
        free(shader_source);
    }
    
    return result;
}

internal void
set_assert_uniform_1i(int program_id, char *uniform_name, int uniform_data)
{
    glUseProgram(program_id);
    GLint uniform_location = glGetUniformLocation(program_id, uniform_name);
    AssertUniformLoc(uniform_location);
    glUniform1i(uniform_location, uniform_data);
}

internal u32
create_quad()
{
    f32 quad[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
    };
    
    u32 vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), &quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    return vao;
}

internal preview_context
setup_preview_window(u32 image_width, u32 image_height)
{
    preview_context result = {};
    
    if(!glfwInit())
    {
        printf("ERROR: GLFW failed to initialize\n");
    }
    
    result.window = glfwCreateWindow(image_width, image_height, "Render Preview", 0, 0);
    glfwMakeContextCurrent(result.window);
    
    GLenum glew_error = glewInit();
    if(glew_error != GLEW_OK)
    {
        printf("ERROR: Glew failed to initialize");
    }
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    char *vs_shader = "#version 330 core\n layout (location = 0) in vec3 aPos; layout (location = 1) in vec2 aTexCoords; out vec2 TexCoords; void main() { TexCoords = aTexCoords; gl_Position = vec4(aPos, 1.0); }";
    char *fs_shader = "#version 330 core\n out vec4 FragColor; in vec2 TexCoords; uniform sampler2D colorBuffer; void main() { FragColor = vec4(texture(colorBuffer, TexCoords).rgb, 1.0); }";
    
    result.screen_quad_vao = create_quad();
    
    u32 vs_id = compile_shader(vs_shader, GL_VERTEX_SHADER);
    u32 fs_id = compile_shader(fs_shader, GL_FRAGMENT_SHADER);
    result.shader_id = link_program(vs_id, fs_id);
    set_assert_uniform_1i(result.shader_id, "colorBuffer", 0);
    
    result.texture_width = image_width;
    result.texture_height = image_height;
    glGenTextures(1, &result.screen_texture);
    glBindTexture(GL_TEXTURE_2D, result.screen_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image_width, image_height, 0, GL_RGB, GL_FLOAT, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glUseProgram(result.shader_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, result.screen_texture);
    glBindVertexArray(result.screen_quad_vao);
    
    return result;
}

internal void
update_preview(preview_context *preview, f32 *pixels)
{
    if(preview->fence_status == GL_SIGNALED)
    {
        //glInvalidateTexImage(frame_texture, 0); // Orphaning may not be neccessary
        glTextureSubImage2D(preview->screen_texture, 0, 0, 0, preview->texture_width, preview->texture_height, GL_RGB, GL_FLOAT, pixels);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        preview->fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glfwSwapBuffers(preview->window);
    }
    
    glGetSynciv(preview->fence, GL_SYNC_STATUS, 1, 0, &preview->fence_status);
}