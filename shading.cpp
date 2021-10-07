#include "shading.h"

using namespace glm;

internal v4
calc_point_light(material material, point_light light, v4 view_point, v4 eyev, v4 surface_normal)
{
    v4 effective_color = material.color * light.intensity;
    v4 ambient_component = effective_color * material.ambient;
    
    v4 light_direction = glm::normalize(light.position - view_point);
    f32 light_dot_normal = glm::max(glm::dot(light_direction, surface_normal), 0.0f);
    v4 diffuse_component = light_dot_normal * material.diffuse * effective_color;
    
    v4 reflect_dir = glm::reflect(-light_direction, surface_normal);
    f32 spec_factor = glm::pow(glm::max(glm::dot(reflect_dir, eyev), 0.0f), material.shininess);
    v4 specular_component = light.intensity * spec_factor * material.specular;
    
    v4 result = v4(v3(ambient_component + diffuse_component + specular_component), 1); 
    
    return result;
}


inline material
default_material()
{
    material m;
    m.color = color(1, 1, 1);
    m.ambient = 0.1f;
    m.diffuse = 0.9f;
    m.specular = 0.9f;
    m.shininess = 200.0f;
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
