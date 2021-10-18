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

internal void write_ppm(f32 *data, int width, int height, f32 gamma, char *fname);
internal pixel_buffer allocate_pixel_buffer(u32 width, u32 height);
internal void clear(pixel_buffer buffer, v4 c);
internal void set_pixel(pixel_buffer *buffer, int x, int y, v4 color);

#endif //MAIN_H
