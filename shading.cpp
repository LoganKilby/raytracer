#include "shading.h"

using namespace glm;

internal v4
calc_point_light(material material, point_light light, v3 view_point, v3 eyev, v3 surface_normal)
{
    v3 normal = v3(surface_normal);
    v3 eye_vector = v3(eyev);
    
    v3 effective_color = v3(material.color) * light.intensity;
    v3 ambient_component = effective_color * material.ambient;
    
    v3 light_direction = glm::normalize(light.position - view_point);
    f32 light_dot_normal = glm::max(glm::dot(light_direction, normal), 0.0f);
    v3 diffuse_component = light_dot_normal * material.diffuse * effective_color;
    
    v3 reflect_dir = glm::reflect(-light_direction, normal);
    f32 spec_factor = glm::pow(glm::max(glm::dot(reflect_dir, eye_vector), 0.0f), material.shininess);
    v3 specular_component = light.intensity * spec_factor * material.specular;
    
    v4 result = v4(v3(ambient_component + diffuse_component + specular_component), 1); 
    
    return result;
}


inline material
default_material()
{
    material m;
    m.color = v4(1, 1, 1, 0);
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
