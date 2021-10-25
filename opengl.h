/* date = October 25th 2021 4:50 pm */

#ifndef OPENGL_H
#define OPENGL_H

struct opengl_render_target // Floating point buffer for storing HDR color values
{
    unsigned int FrameBuffer; // GL_FRAMEBUFFER
    unsigned int RenderBuffer; // GL_FRAMEBUFFER
    unsigned int ColorBuffer; // GL_TEXTURE_2D
};

#endif //OPENGL_H
