/* date = October 6th 2021 10:17 am */

#ifndef SHADING_H
#define SHADING_H

struct point_light
{
    v4 intensity;
    v4 position;
};

struct material
{
    v4 color;
    f32 ambient;
    f32 diffuse;
    f32 specular;
    f32 shininess;
};

inline material default_material();

#endif //SHADING_H
