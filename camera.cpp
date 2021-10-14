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
