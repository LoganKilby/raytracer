#include "camera.h"

internal void
camera_compute_basis(camera *camera)
{
    if(float_equal(camera->position.x, camera->look_at.x) &&
       float_equal(camera->position.z, camera->look_at.z))
    {
        camera->w = v3(0, 1, 0);
        camera->u = v3(0, 0, 1);
        camera->v = v3(1, 0, 0);
        return;
    }
    
    camera->w = camera->position - camera->look_at;
    camera->w = glm::normalize(camera->w);
    camera->u = glm::cross(camera->up, camera->w);
    camera->u = glm::normalize(camera->u);
    camera->v = glm::cross(camera->w, camera->u);
}

internal v3
thin_lens_ray_direction(thin_lens_camera *camera, v2 lens_position, v2 pixel)
{
    v2 p;
    p.x = pixel.x * camera->focal_length / camera->vp_distance;
    p.y = pixel.y * camera->focal_length / camera->vp_distance;
    
    v3 result = v3((p.x - lens_position.x) * camera->u +
                   (p.y - lens_position.y) * camera->v -
                   (camera->focal_length * camera->w));
    
    return glm::normalize(result);
}

inline v3
pinhole_ray_direction(pinhole_camera *camera, v2 point)
{
    return glm::normalize(point.x * camera->u + 
                          point.y * camera->v - 
                          camera->vp_distance * camera->w);
}

inline void
zoom_pinhole_camera(pinhole_camera *camera, f32 zoom)
{
    camera->zoom = 1 / zoom;
}
