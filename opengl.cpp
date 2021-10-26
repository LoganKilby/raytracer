#define AssertUniformLoc(Integer) if(Integer == -1) {*(int *)0 = 0;}

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
load_compile_shader(char *file_name, GLenum shader_type)
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
    
    char *shader_source = os_read_entire_file(file_name);
    
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
            
            fprintf(stderr, "\nERROR: Shader compilation error (%s): %s\n", shader_type_string, file_name);
            fprintf(stderr, "%s", msg_buffer);
            fprintf(stderr, "\n%s\n\n", shader_source);
            
            free(msg_buffer);
        }
        
        free(shader_source);
    }
    
    return result;
}

internal void
set_assert_uniform_1i(int Program, char *Name, int Data)
{
    glUseProgram(Program);
    GLint UniformLocation = glGetUniformLocation(Program, Name);
    AssertUniformLoc(UniformLocation);
    glUniform1i(UniformLocation, Data);
}

internal u32
create_quad()
{
    f32 quad[] = {
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
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
