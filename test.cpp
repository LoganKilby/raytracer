#include "types.h"
#include "macros.h"
#include "math_functions.h"
#include "stdio.h"
#include "test.h"

#include "ray.cpp"

void scale_inverse()
{
    mat4 scale = mat4_scale(mat4_identity(), {2, 3, 4});
    mat4 inv = mat4_inverse(scale);
    v4 v = vector(-4, 6, 8, 0);
    v = v4_mat4_multiply(v, inv);
    bool result = (v.x == -2) && (v.y == 2) && (v.z == 2);
    TrackResult(result);
}

void x_rotation_transform()
{
    v4 p = point(0, 1, 0, 1);
    mat4 halfq = mat4_rotation(mat4_identity(), {1, 0, 0}, M_PI / 4.0f);
    mat4 fullq = mat4_rotation(mat4_identity(), {1, 0, 0}, M_PI / 2.0f);
    v4 q0 = v4_mat4_multiply(p, halfq);
    v4 q1 = v4_mat4_multiply(p, fullq);
    f32 e = FLT_EPSILON;
    
    f32 sr2 = sqrt(2)/2.0f;
    
    bool x0 = (q0.x < FLT_EPSILON);
    bool x1 = (q0.y == sr2);
    bool x2 = (q0.z == sr2);
    
    bool y0 = (q1.x < FLT_EPSILON);
    bool y1 = (q1.y < FLT_EPSILON);
    bool y2 = (q1.z == 1);
    
    bool result = x0 && x1 && x2 && y0 && y1 && y2;
    TrackResult(result);
}

void shear()
{
    v4 p = point(2, 3, 4, 1);
    mat4 s = shear_matrix(0, 1, 0, 0, 0, 0);
    p = v4_mat4_multiply(p, s);
    bool result = (p.x == 6) && (p.y == 3) && (p.z == 4);
    TrackResult(result);
}

void matrix_multiply()
{
    mat4 a = new_mat4(1, 2, 3, 4,
                      5, 6, 7, 8,
                      9, 8, 7, 6,
                      5, 4, 3, 2);
    mat4 b = new_mat4(-2, 1, 2, 3,
                      3, 2, 1, -1,
                      4, 3, 6, 5,
                      1, 2, 7, 8);
    
    mat4 c = mat4_multiply(a, b);
    mat4 ce = new_mat4(20, 22, 50, 48,
                       44, 54, 114, 108,
                       40, 58, 110, 102,
                       16, 26, 46, 42);
    bool result = mat4_equality(c, ce);
    TrackResult(result);
    
    c = new_mat4(1, 2, 3, 4,
                 2, 4, 4, 2,
                 8, 6, 4, 1,
                 0, 0, 0, 1);
    v4 v = point(1, 2, 3, 1);
    v = v4_mat4_multiply(v, c);
    result = v4_equality(v, {18, 24, 33, 1});
    TrackResult(result);
}

void sequential_transforms()
{
    mat4 a = mat4_rotation(mat4_identity(), {1, 0, 0}, M_PI / 2);
    mat4 b = mat4_scale(mat4_identity(), {5, 5, 5});
    mat4 c = mat4_translation(mat4_identity(), {10, 5, 7});
    v4 v0 = point(1, 0, 1, 1);
    
    v4 v1 = v4_mat4_multiply(v0, a);
    bool r = v4_equality(v1, point(1, -1, 0, 1));
    v4 v2 = v4_mat4_multiply(v1, b);
    bool s = v4_equality(v2, point(5, -5, 0, 1));
    v4 v3 = v4_mat4_multiply(v2, c);
    bool t = v4_equality(v3, point(15, 0, 7, 1));
    
    bool result = r && s && t;
    TrackResult(result);
}

void chain_transforms()
{
    mat4 a = mat4_rotation(mat4_identity(), {1, 0, 0}, M_PI / 2);
    mat4 b = mat4_scale(mat4_identity(), {5, 5, 5});
    mat4 c = mat4_translation(mat4_identity(), {10, 5, 7});
    mat4 T = c;
    T = mat4_multiply(T, b);
    T = mat4_multiply(T, a);
    v4 v0 = point(1, 0, 1, 1);
    
    v4 v1 = v4_mat4_multiply(v0, T);
    bool result = v4_equality(v1, point(15, 0, 7, 1));
    TrackResult(result);
}

void chain_transforms2()
{
    mat4 c = mat4_translation(mat4_identity(), {10, 5, 7});
    mat4 b = mat4_scale(mat4_identity(), {5, 5, 5});
    mat4 a = mat4_rotation(mat4_identity(), {1, 0, 0}, M_PI / 2);
    
    mat4 model = mat4_multiply(c, b);
    model = mat4_multiply(model, a);
    
    v4 v = point(1, 0, 1, 1);
    v4 v1 = v4_mat4_multiply(v, model);
    bool result = v4_equality(v1, point(15, 0, 7, 1));
    TrackResult(result);
}

void ray_hit_test()
{
    f32 result = ray_hit(1, 2);
    TrackResult(result == 1);
    
    f32 result1 = ray_hit(-1, 1);
    TrackResult(result == 1);
    
    f32 result2 = ray_hit(-1, -1);
    TrackResult(result2 == 0);
}

void ray_sphere_hit()
{
    sphere s = new_sphere({0, 0, 0}, 1);
    ray r = new_ray({0, 0, -5}, {0, 0, 1});
    
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = i.t0 == 4 && i.t1 == 6;
    TrackResult(collision && result);
}

void ray_sphere_mult_intersection()
{
    sphere s = new_sphere({0, 0, 0}, 1);
    ray r = new_ray({0, 1, -5}, {0, 0, 1});
    
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = i.t0 == 5 && i.t1 == 5;
    TrackResult(collision && result);
}

void ray_sphere_miss()
{
    sphere s = new_sphere({0, 0, 0}, 1);
    ray r = new_ray({0, 2, -5}, {0, 0, 1});
    
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = i.t0 == 0 && i.t1 == 0;
    TrackResult(!collision && result);
}

void ray_inside_sphere()
{
    sphere s = new_sphere({0, 0, 0}, 1);
    ray r = new_ray({0, 0, 0}, {0, 0, 1});
    
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = i.t0 == -1 && i.t1 == 1;
    TrackResult(collision && result);
}

void ray_behind_sphere()
{
    sphere s = new_sphere({0, 0, 0}, 1);
    ray r = new_ray({0, 0, 5}, {0, 0, 1});
    
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = i.t0 == -6 && i.t1 == -4;
    TrackResult(collision && result);
}

void translate_ray()
{
    ray r;
    r.origin = point(1, 2, 3, 1);
    r.direction = vector(0, 1, 0, 0);
    mat4 m = mat4_translation({3, 4, 5});
    ray r1 = ray_translate(r, m);
    bool result = (v4_equality(r1.origin, {4, 6, 8, 0})) && (v4_equality(r1.direction, {0, 1, 0, 0}));
}

void scale_ray()
{
    ray r;
    r.origin = point(1, 2, 3, 1);
    r.direction = vector(0, 1, 0, 0);
    mat4 m = mat4_scale({2, 3, 4});
    ray r1 = ray_translate(r, m);
    bool result = (v4_equality(r1.origin, {2, 6, 12, 0})) && (v4_equality(r1.direction, {0, 3, 0, 0}));
}

void scaled_ray_sphere_intersect()
{
    ray r = new_ray({0, 0, -5}, {0, 0, 1});
    sphere s = new_sphere({0, 0, 0}, 1);
    s.transform = mat4_scale({2, 2, 2});
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = (i.t0 == 3) && (i.t1 == 7);
    TrackResult(collision && result);
}

void translated_ray_sphere_intersection()
{
    ray r = new_ray({0, 0, -5}, {0, 0, 1});
    sphere s = new_sphere({0, 0, 0}, 1);
    s.transform = mat4_translation({5, 0, 0});
    intersection i;
    bool collision = ray_sphere_intersection(r, s, &i);
    bool result = (i.t0 == 0) && (i.t1 == 0);
    TrackResult(!collision && result);
}

int main()
{
    scale_inverse();
    x_rotation_transform();
    shear();
    matrix_multiply();
    sequential_transforms();
    chain_transforms();
    chain_transforms2();
    ray_hit_test();
    ray_sphere_hit();
    ray_sphere_mult_intersection();
    ray_sphere_miss();
    ray_inside_sphere();
    ray_behind_sphere();
    translate_ray();
    scale_ray();
    scaled_ray_sphere_intersect();
    translated_ray_sphere_intersection();
}