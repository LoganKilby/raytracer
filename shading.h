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

inline material
default_material()
{
    material m;
    m.color = color(1, 1, 1);
    m.ambient = 0.1f;
    m.diffuse = 0.9f;
    m.specular = 0.9f;
    m.shininess = 100.0f;
    return m;
}

inline v4
color_multiply(v4 a, v4 b)
{
    v4 result;
    result.r = a.r * b.r;
    result.g = a.g * b.g;
    result.b = a.b * b.b;
    result.w = a.a * b.a;
    return result;
}

#endif //SHADING_H
