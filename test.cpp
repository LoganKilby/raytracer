#include "types.h"
#include "macros.h"
#include "math_lib.h"
#include "stdio.h"
#include "test.h"
#include "ray.cpp"
#include "shading.cpp"

void matrix_mult()
{
    glm::mat4 a = glm::transpose(glm::mat4(1, 2, 3, 4,
                                           5, 6, 7, 8,
                                           9, 8, 7, 6,
                                           5, 4, 3, 2));
    glm::mat4 b = glm::transpose(glm::mat4(-2, 1, 2, 3,
                                           3, 2, 1, -1,
                                           4, 3, 6, 5,
                                           1, 2, 7, 8));
    glm::mat4 c = a * b;
    glm::mat4 d = glm::transpose(glm::mat4(20, 22, 50, 48,
                                           44, 54, 114, 108,
                                           40, 58, 110, 102,
                                           16, 26, 46, 42));
    
    bool result = mat4_equality(c, d);
    TrackResult(result);
}

void rotation()
{
    glm::vec4 p = point(0, 1, 0);
    glm::mat4 halfq = glm::rotate(glm::mat4(1.0f), (float)M_PI/4, glm::vec3(1, 0, 0));
    glm::mat4 fullq = glm::rotate(glm::mat4(1.0f), (float)M_PI/2, glm::vec3(1, 0, 0));
    
    glm::vec4 q0 = p * glm::transpose(halfq);
    glm::vec4 q1 = p * glm::transpose(fullq);
    
    f32 f = sqrt(2.)/2;
    bool result = 1;
    result &= v4_equality(q0, point(0, f, f));
    result &= v4_equality(q1, point(0, 0, 1));
    TrackResult(result);
}

void sphere_normal_translated()
{
    sphere s = new_sphere();
    s.transform = glm::translate(s.transform, v3(0, 1, 0));
    glm::mat4 m = s.transform;
    
    v4 n = sphere_normal(s.transform, point(0, 1.70711, -0.70711));
    
    v4 wp = point(0, 1.70711, -0.70711);
    wp = glm::inverse(s.transform) * wp;
    wp = glm::transpose(glm::inverse(s.transform)) * wp;
    wp.w = 0;
    
    bool result = v3_equality(n, vector(0, 0.70711, -0.70711));
    TrackResult(result);
}

void sphere_normal_rotated()
{
    sphere s = new_sphere();
    s.transform = glm::scale(s.transform, v3(1, 0.5f, 1));
    s.transform = glm::rotate(s.transform, (float)(M_PI / 5.0f), v3(0, 0, 1));
    
    v4 n = sphere_normal(s.transform, point(0, (float)sqrt(2.0f)/2.0f, -(float)sqrt(2.0f)/2.0f));
    n = glm::normalize(n);
    bool result = v4_equality(n, vector(0, 0.97014, -0.24254));
    TrackResult(result);
}

void reflect_test()
{
    v4 v = vector(1, -1, 0);
    v4 n = vector(0, 1, 0);
    v4 r = glm::reflect(v, n);
    bool result = v4_equality(r, vector(1, 1, 0));
    TrackResult(result);
}

void reflect_test2()
{
    // slanted surface
    f32 f = sqrt(2)/2;
    v4 v = vector(0, -1, 0);
    v4 n = vector(f, f, 0);
    v4 r = glm::reflect(v, n);
    bool result = v4_equality(r, vector(1, 0, 0));
    TrackResult(result);
}

void color_calc0()
{
    v4 view_dir = vector(0, 0, -1);
    v4 surface_normal = vector(0, 0, -1);
    point_light light;
    light.position = point(0, 0, -10);
    light.intensity = glm::vec4(1.0f);
    material m = default_material();
    // after lighting
    
    v4 position = glm::vec4(0.0f);
    v4 pixel_color = calc_point_light(m, light, position, view_dir, surface_normal);
    bool result = v4_equality(pixel_color, v4(1.9f, 1.9f, 1.9f, 1.0f));
    TrackResult(result);
}

void color_calc1()
{
    f32 f = sqrt(2)/2;
    v4 eye = vector(0, f, -f);
    v4 surface_normal = vector(0, 0, -1);
    point_light light;
    light.position = point(0, 0, -10);
    light.intensity = glm::vec4(1.0f);
    material m = material();
    v4 position = point(0, 0, 0);
    v4 pixel_color = calc_point_light(m, light, position, eye, surface_normal);
    bool result = v4_equality(pixel_color, v4(1, 1, 1, 1));
    TrackResult(result);
}

int main()
{
    matrix_mult();
    rotation();
    sphere_normal_translated();
    sphere_normal_rotated();
    reflect_test();
    reflect_test2();
    color_calc0();
    color_calc1();
    printf("%d tests completed.\n", tests_ran);
}