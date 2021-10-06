#include "shading.h"

internal v4
calc_point_light(material material, point_light light, v4 view_point, v4 view_direction, v4 surface_normal)
{
    v4 effective_color = material.color * light.intensity;
    v4 ambient_component = effective_color * material.ambient;
    
    v4 light_direction = glm::normalize(light.position - view_point);
    f32 light_dot_normal = glm::max(glm::dot(light_direction, surface_normal), 0.0f);
    v4 diffuse_component = light_dot_normal * material.diffuse * effective_color;
    
    
    v4 reflect_dir = glm::reflect(-light_direction, surface_normal);
    f32 reflect_dot_eye = glm::pow(glm::max(glm::dot(reflect_dir, view_direction), 0.0f), material.shininess);
    v4 specular_component = reflect_dot_eye * light.intensity;
    
    
    return v4(v3(ambient_component + diffuse_component + specular_component), 1);
}
