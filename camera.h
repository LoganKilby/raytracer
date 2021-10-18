/* date = October 14th 2021 4:04 pm */

#ifndef CAMERA_H
#define CAMERA_H

struct camera
{
    v3 position;
    v3 look_at;
    v3 z_axis;
    v3 x_axis;
    v3 y_axis;
    v3 up;
    v3 u, v, w;
};

struct pinhole_camera : camera
{
    f32 vp_distance;
    f32 zoom;
};

struct thin_lens_camera : pinhole_camera
{
    f32 lens_radius;
    f32 focal_length;
};

#endif //CAMERA_H
