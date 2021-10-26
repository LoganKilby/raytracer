#include "opengl.h"

#define AssertUniformLoc(Integer) if(Integer == -1) {*(int *)0 = 0;}

// NOTE: Example of calculating tangent and bitanent vectors
internal void 
RenderQuad()
{
    static unsigned int quadVAO, quadVBO = 0;
    if (quadVAO == 0)
    {
        // Quad without tangent/bitangent
        float quadVertices[] =
        {
            -1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // 1
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,// 2
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // 3
            
            -1.0f, 1.0f, 0.0f,  0.0f, 1.0f, // 1
            1.0f, -1.0f, 0.0f,  1.0f, 0.0f, // 3
            1.0f, 1.0f, 0.0f,   1.0f, 1.0f   // 4
        };
        
        
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        
    }
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// TODO: Probably want to take an array of compiled shaders
// TODO: Probably don't need a program struct
internal u32
CreateShaderProgram(GLuint VertexShaderID, GLuint FragmentShaderID)
{
    u32 Result = glCreateProgram();
    
    glAttachShader(Result, VertexShaderID);
    glAttachShader(Result, FragmentShaderID);
    glLinkProgram(Result);
    
    int LinkStatus;
    glGetProgramiv(Result, GL_LINK_STATUS, &LinkStatus);
    if(LinkStatus == GL_FALSE)
    {
        fprintf(stderr, "ERROR: Shader link error (Program ID: %d)\n", Result);
        int LogLength;
        glGetProgramiv(Result, GL_INFO_LOG_LENGTH, &LogLength);
        if(LogLength)
        {
            char *LogBuffer = (char *)malloc(LogLength);
            memset(LogBuffer, 0, LogLength);
            glGetProgramInfoLog(Result, LogLength, NULL, LogBuffer);
            fprintf(stderr, "%s\n\n", LogBuffer);
            free(LogBuffer);
        }
    }
    
    // NOTE: glDeleteShader sets a flag for deletion. A shader won't be deleted while attached to a program.
    
    glDeleteShader(VertexShaderID); 
    glDeleteShader(FragmentShaderID);
    
    return Result;
}

static char *
ReadEntireFileToString(char *Filename)
{
    HANDLE FileHandle = CreateFileA(Filename,
                                    GENERIC_READ,
                                    0,
                                    0,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, // TODO: Read only?
                                    0);
    
    if(FileHandle == INVALID_HANDLE_VALUE)
    {
        // TODO: Logging
        fprintf(stderr, "WARNING: Windows could not open the file %s.\n", Filename);
        return 0;
    }
    
    unsigned int FileSizeInBytes = GetFileSize(FileHandle, 0);
    char *FileBuffer = 0;
    if(FileSizeInBytes)
    {
        FileBuffer = (char *)malloc(FileSizeInBytes + 1);
        memset(FileBuffer, 0, FileSizeInBytes + 1);
        
        unsigned long int BytesRead;
        bool FileRead = ReadFile(FileHandle, FileBuffer, FileSizeInBytes, &BytesRead, 0);
        if(!FileRead)
        {
            // TODO: Logging
            fprintf(stderr, "WARNING: Windows encountered an error while reading from \"%s\". File read aborted.", Filename);
            free(FileBuffer);
            FileBuffer = 0;
        }
    }
    
    CloseHandle(FileHandle);
    
    return FileBuffer;
}

internal u32
LoadAndCompileShader(char *Filename, GLenum ShaderType)
{
    u32 ResultID = 0;
    char *ShaderTypeString;
    switch(ShaderType)
    {
        case GL_VERTEX_SHADER:
        ShaderTypeString = "GL_VERTEX_SHADER";
        break;
        case GL_FRAGMENT_SHADER:
        ShaderTypeString = "GL_FRAGMENT_SHADER";
        break;
        default:
        {
            ShaderTypeString = "UNKNOWN_SHADER_TYPE";
        }
    }
    
    char *ShaderSource = ReadEntireFileToString(Filename);
    
    if(ShaderSource)
    {
        ResultID = glCreateShader(ShaderType);
        glShaderSource(ResultID, 1, &ShaderSource, NULL);
        glCompileShader(ResultID);
        
        int CompilationStatus;
        glGetShaderiv(ResultID, GL_COMPILE_STATUS, &CompilationStatus);
        if(CompilationStatus == GL_FALSE)
        {
            int LogLength;
            glGetShaderiv(ResultID, GL_INFO_LOG_LENGTH, &LogLength);
            char *LogBuffer = (char *)malloc(LogLength);
            memset(LogBuffer, 0, LogLength);
            glGetShaderInfoLog(ResultID, LogLength, NULL, LogBuffer);
            
            fprintf(stderr, "\nERROR: Shader compilation error (%s): %s\n", ShaderTypeString, Filename);
            fprintf(stderr, "%s", LogBuffer);
            fprintf(stderr, "\n%s\n\n", ShaderSource);
            
            free(LogBuffer);
        }
        
        free(ShaderSource);
    }
    
    return ResultID;
}

internal void
SetUniform1i(int Program, char *Name, int Data)
{
    glUseProgram(Program);
    GLint UniformLocation = glGetUniformLocation(Program, Name);
    AssertUniformLoc(UniformLocation);
    glUniform1i(UniformLocation, Data);
}

internal unsigned int
CreateScreenRenderQuad()
{
    float ScreenRenderQuad[] = {
#if 0
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
#endif
        // positions        // texture Coords
        -1.0f,  1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    };
    
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ScreenRenderQuad), &ScreenRenderQuad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    return VAO;
}
