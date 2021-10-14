/* date = October 14th 2021 4:04 pm */

#ifndef CAMERA_H
#define CAMERA_H

struct camera
{
    v3 position;
    v3 look_at;
    v3 up;
    v3 u, v, w;
};

struct pinhole_camera : camera
{
    f32 d;
    f32 zoom;
};

inline v3
pinhole_camera_ray_direction(v2 point, pinhole_camera *camera)
{
    return glm::normalize(point.x * camera->u + 
                          point.y * camera->v - 
                          camera->d * camera->w);
}

inline void
zoom_pinhole_camera(pinhole_camera *camera, f32 zoom)
{
    camera->zoom = 1 / zoom;
}
#endif //CAMERA_H
