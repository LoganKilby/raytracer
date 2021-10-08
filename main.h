/* date = October 8th 2021 1:35 pm */

#ifndef MAIN_H
#define MAIN_H

struct pixel_buffer
{
    // NOTE: 4 floats per pixel
    f32 *data;
    u32 width;
    u32 height;
    u32 pitch;
};

struct view_plane
{
    f32 pixel_size;
    
    // NOTE: Default gamma in most cases in 1.0
    f32 gamma; 
    f32 inv_gamma;
};

struct world
{
    view_plane vp;
};

internal void write_ppm(f32 *data, int width, int height, f32 gamma, char *fname);
internal pixel_buffer new_pixel_buffer(u32 width, u32 height);
internal void clear(pixel_buffer buffer, v4 c);
internal void set_pixel(pixel_buffer buffer, f32 x, f32 y, v4 color);

inline void
set_gamma(view_plane *vp, f32 gamma)
{
    vp->gamma = gamma;
    vp->inv_gamma = 1.0f / gamma;
}

#endif //MAIN_H
