#include "types.h"
#include "macros.h"
#include "math_lib.h"
#include "stdio.h"
#include "test.h"
#include "ray.cpp"

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

void sphere_normal()
{
    sphere s = new_sphere();
    v4 nx = sphere_normal(s, point(1, 0, 0));
    v4 ny = sphere_normal(s, point(0, 1, 0));
    v4 nz = sphere_normal(s, point(0, 0, 1));
    
    f32 sqrt3 = sqrt(3) / 3;
    v4 na = sphere_normal(s, point(sqrt3, sqrt3, sqrt3));
    
    bool result = v4_equality(nx, vector(1, 0, 0));
    result &= v4_equality(ny, vector(0, 1, 0));
    result &= v4_equality(nz, vector(0, 0, 1));
    result &= v4_equality(na, vector(sqrt3, sqrt3, sqrt3));
    result &= v4_equality(na, glm::normalize(na));
    
    TrackResult(result);
}

int main()
{
    matrix_mult();
    rotation();
    sphere_normal();
    printf("%d tests ran.\n", tests_ran);
}