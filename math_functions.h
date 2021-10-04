/* date = October 2nd 2021 4:18 pm */

#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#define _USE_MATH_DEFINES
#include "math.h"
#include "float.h"

struct v3
{
    float x, y, z;
};

struct v2
{
    float x, y;
};

struct u2
{
    u32 x;
    u32 y;
};

struct v4
{
    float x, y, z, w;
    
    v4(f32 x, f32 y, f32 z, f32 w)
    {
        x = x; y = y; z = z; w = w;
    }
    
    v4()
    {
        x = 0; y = 0; z = 0; w = 0;
    }
    
    v4(v3 a, f32 w)
    {
        x = a.x; y = a.y; z = a.z; w = w;
    }
};

inline v4
vector(f32 x, f32 y, f32 z, f32 w)
{ 
    Assert(w == 0);
    v4 result; 
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return(result); 
};

inline v4
vector(f32 x, f32 y, f32 z)
{ 
    v4 result; 
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 0;
    return(result); 
};

inline v3
vector3(v4 v)
{
    v3 result;
    result.x = v.x;
    result.y = v.y;
    result.z = v.z;
    return(result);
}

inline v4
point(f32 x, f32 y, f32 z, f32 w)
{
    Assert(w == 1);
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return(result);
}

inline v4
point(f32 x, f32 y, f32 z)
{
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 1;
    return(result);
}

inline v3
point3(f32 x, f32 y, f32 z)
{
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return(result);
}

inline v4
v4_sub(v4 u, v4 v)
{
    v4 result;
    result.x = u.x - v.x;
    result.y = u.y - v.y;
    result.z = u.z - v.z;
    result.w = u.w - v.w;
    return(result);
}

inline v3
v3_sub(v3 u, v3 v)
{
    v3 result;
    result.x = u.x - v.x;
    result.y = u.y - v.y;
    result.z = u.z - v.z;
    return(result);
}

inline v4
v4_add(v4 u, v4 v)
{
    v4 result;
    result.x = u.x + v.x;
    result.y = u.y + v.y;
    result.z = u.z + v.z;
    result.w = u.w + v.w;
    return(result);
}

inline v4
v4_negate(v4 a)
{
    v4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return(result);
}

inline v4
v4_smul(v4 v, f32 a)
{
    v4 result;
    result.x = v.x * a;
    result.y = v.y * a;
    result.z = v.z * a;
    result.w = v.w * a;
    return(result);
}

inline v4
v4_sdiv(v4 v, f32 a)
{
    float i = 1 / a;
    
    v4 result;
    result.x = v.x * i;
    result.y = v.y * i;
    result.z = v.z * i;
    result.w = v.w * i;
    return(result);
}

inline f32
v4_dot(v4 u, v4 v)
{
    f32 result;
    result = u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
    return(result);
}

inline v4
v4_cross(v4 u, v4 v)
{
    v4 result;
    result.x = u.y * v.z - u.z * v.y;
    result.y = u.z * v.x - u.x * v.z;
    result.z = u.x * v.y - u.y * v.x;
    result.w = 0;
    return result;
}

inline f32
v4_magnitude(v4 v)
{
    AssertZero(v.w);
    f32 result;
    result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    return(result);
}

inline v4
v4_normalize(v4 v)
{
    f32 inverse_magnitude = 1 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    
    v4 result;
    result.x = v.x * inverse_magnitude;
    result.y = v.y * inverse_magnitude;
    result.z = v.z * inverse_magnitude;
    result.w = 0;
    return(result);
}

inline v4
v4_normalize(v3 v)
{
    f32 inverse_magnitude = 1 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    
    v4 result;
    result.x = v.x * inverse_magnitude;
    result.y = v.y * inverse_magnitude;
    result.z = v.z * inverse_magnitude;
    result.w = 0;
    return(result);
}

inline v4
v4_blend(v4 u, v4 v)
{
    v4 result;
    result.x = u.x * v.x;
    result.y = u.y * v.y;
    result.z = u.z * v.z;
    result.w = u.w * v.w;
    return result;
}

struct mat4
{
    f32 m00, m01, m02, m03,
    m10, m11, m12, m13,
    m20, m21, m22, m23,
    m30, m31, m32, m33;
};

inline mat4
new_mat4(f32 m00, f32 m01, f32 m02, f32 m03,
         f32 m10, f32 m11, f32 m12, f32 m13,
         f32 m20, f32 m21, f32 m22, f32 m23,
         f32 m30, f32 m31, f32 m32, f32 m33)
{
    mat4 result;
    result.m00 = m00; result.m01 = m01; result.m02 = m02; result.m03 = m03;
    result.m10 = m10; result.m11 = m11; result.m12 = m12; result.m13 = m13;
    result.m20 = m20; result.m21 = m21; result.m22 = m22; result.m23 = m23;
    result.m30 = m30; result.m31 = m31; result.m32 = m32; result.m33 = m33;
    return(result);
}

inline mat4 
mat4_identity()
{
    mat4 result = {};
    result.m00 = 1;
    result.m11 = 1;
    result.m22 = 1;
    result.m33 = 1;
    return(result);
}

inline mat4
mat4_multiply(mat4 a, mat4 b)
{
    f32 *m = &a.m00;
    f32 *s = &b.m00;
    
    mat4 result;
    f32 *r = &result.m00;
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            r[row * 4 + col] = 
                m[row * 4 + 0] * s[col + (4 * 0)] +
                m[row * 4 + 1] * s[col + (4 * 1)] +
                m[row * 4 + 2] * s[col + (4 * 2)] +
                m[row * 4 + 3] * s[col + (4 * 3)];
        }
    }
    
    return(result);
}

inline mat4
mat4_translation(mat4 m, v3 t)
{
    mat4 result = mat4_identity();
    result.m03 = t.x;
    result.m13 = t.y;
    result.m23 = t.z;
    result = mat4_multiply(result, m);
    return(result);
}

inline mat4
mat4_translation(v3 t)
{
    mat4 result = mat4_identity();
    result.m03 = t.x;
    result.m13 = t.y;
    result.m23 = t.z;
    return(result);
}

inline mat4
mat4_translation(f32 x, f32 y, f32 z)
{
    mat4 result = mat4_identity();
    result.m03 = x;
    result.m13 = y;
    result.m23 = z;
    return(result);
}

inline mat4
mat4_scale(mat4 m, v3 s)
{
    mat4 result = mat4_identity();
    result.m00 = s.x;
    result.m11 = s.y;
    result.m22 = s.z;
    result = mat4_multiply(result, m);
    return(result);
}

inline mat4
mat4_scale(v3 s)
{
    mat4 result = mat4_identity();
    result.m00 = s.x;
    result.m11 = s.y;
    result.m22 = s.z;
    return(result);
}

inline mat4
mat4_scale(f32 x, f32 y, f32 z)
{
    mat4 result = mat4_identity();
    result.m00 = x;
    result.m11 = y;
    result.m22 = z;
    return(result);
}

inline mat4
mat4_rotation(mat4 m, v3 axis, f32 radians)
{
    Assert(axis.x + axis.y + axis.z == 1);
    
    mat4 result = mat4_identity();
    if(axis.x)
    {
        result.m11 = cos(radians);
        result.m12 = -sin(radians);
        result.m21 = sin(radians);
        result.m22 = cos(radians);
    }
    else if(axis.y)
    {
        result.m00 = cos(radians);
        result.m02 = sin(radians);
        result.m20 = -sin(radians);
        result.m22 = cos(radians);
    }
    else if(axis.z)
    {
        result.m00 = cos(radians);
        result.m01 = -sin(radians);
        result.m10 = sin(radians);
        result.m11 = cos(radians);
    }
    
    result = mat4_multiply(result, m);
    return(result);
}

inline mat4
mat4_shear(mat4 m, f32 xy, f32 xz, f32 yx, f32 yz, f32 zx, f32 zy)
{
    mat4 n = mat4_identity();
    n.m01 = xy;
    n.m02 = xz;
    n.m10 = yx;
    n.m12 = yz;
    n.m20 = zx;
    n.m21 = zy;
    
    return mat4_multiply(m, n);
}

inline mat4
mat4_shear(f32 xy, f32 xz, f32 yx, f32 yz, f32 zx, f32 zy)
{
    mat4 n = mat4_identity();
    n.m01 = xy;
    n.m02 = xz;
    n.m10 = yx;
    n.m12 = yz;
    n.m20 = zx;
    n.m21 = zy;
    
    return n;
}

inline mat4
mat4_rotation(v3 axis, f32 radians)
{
    Assert(axis.x + axis.y + axis.z == 1);
    
    mat4 result = mat4_identity();
    if(axis.x)
    {
        result.m11 = cos(radians);
        result.m12 = -sin(radians);
        result.m21 = sin(radians);
        result.m22 = cos(radians);
    }
    else if(axis.y)
    {
        result.m00 = cos(radians);
        result.m02 = sin(radians);
        result.m20 = -sin(radians);
        result.m22 = cos(radians);
    }
    else if(axis.z)
    {
        result.m00 = cos(radians);
        result.m01 = -sin(radians);
        result.m10 = sin(radians);
        result.m11 = cos(radians);
    }
    
    return(result);
}

inline mat4
shear_matrix(f32 xy, f32 xz,
             f32 yx, f32 yz,
             f32 zx, f32 zy)
{
    mat4 result = mat4_identity();
    result.m01 = xy;
    result.m02 = xz;
    result.m10 = yx;
    result.m12 = yz;
    result.m20 = zx;
    result.m21 = zy;
    return(result);
}

inline mat4
mat4_transpose(mat4 m)
{
    mat4 result;
    result.m00 = m.m00;
    result.m01 = m.m10;
    result.m02 = m.m20;
    result.m03 = m.m30;
    
    result.m10 = m.m01;
    result.m11 = m.m11;
    result.m12 = m.m21;
    result.m13 = m.m31;
    
    result.m20 = m.m02;
    result.m21 = m.m12;
    result.m22 = m.m22;
    result.m23 = m.m32;
    
    result.m30 = m.m03;
    result.m31 = m.m13;
    result.m32 = m.m23;
    result.m33 = m.m33;
    
    return(result);
}

inline v4
v4_mat4_multiply(v4 v, mat4 m)
{
    v4 result;
    result.x = v.x * m.m00 + v.y * m.m01 + v.z * m.m02 + v.w * m.m03;
    result.y = v.x * m.m10 + v.y * m.m11 + v.z * m.m12 + v.w * m.m13;
    result.z = v.x * m.m20 + v.y * m.m21 + v.z * m.m22 + v.w * m.m23;
    result.w = v.x * m.m30 + v.y * m.m31 + v.z * m.m32 + v.w * m.m33;
    return(result);
}

struct mat3
{
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
};

inline mat3
mat3_transpose(mat3 m)
{
    mat3 result;
    result.m00 = m.m00;
    result.m01 = m.m10;
    result.m02 = m.m20;
    
    result.m10 = m.m01;
    result.m11 = m.m11;
    result.m12 = m.m21;
    
    result.m20 = m.m02;
    result.m21 = m.m12;
    result.m22 = m.m22;
    
    return(result);
}

inline v3
v3_mat3_multiply(v3 v, mat3 m)
{
    v3 result;
    result.x = v.x * m.m00 + v.y * m.m01 + v.z * m.m02;
    result.y = v.x * m.m10 + v.y * m.m11 + v.z * m.m12;
    result.z = v.x * m.m20 + v.y * m.m21 + v.z * m.m22;
    return(result);
}

inline mat3
new_mat3(f32 m00, f32 m01, f32 m02,
         f32 m10, f32 m11, f32 m12,
         f32 m20, f32 m21, f32 m22)
{
    mat3 result;
    result.m00 = m00; result.m01 = m01; result.m02 = m02;
    result.m10 = m10; result.m11 = m11; result.m12 = m12;
    result.m20 = m20; result.m21 = m21; result.m22 = m22;
    return(result);
}

inline mat3
new_mat3(mat4 a)
{
    mat3 result;
    result.m00 = a.m00; result.m01 = a.m01; result.m02 = a.m02;
    result.m10 = a.m10; result.m11 = a.m11; result.m12 = a.m12;
    result.m20 = a.m20; result.m21 = a.m21; result.m22 = a.m22;
    return result;
}

inline mat3
mat3_identity()
{
    mat3 result = {};
    result.m00 = 1;
    result.m11 = 1;
    result.m22 = 1;
    return(result);
}

inline mat3
mat3_submat4(mat4 matrix, int row_ignore = 3, int col_ignore = 3)
{
    mat3 result = {};
    f32 *r = &result.m00;
    f32 *m = &matrix.m00;
    
    int sub_index = 0;
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            if((row != row_ignore) && (col != col_ignore))
            {
                r[sub_index++] = m[4 * row + col];
            }
        }
    }
    
    return(result);
}

struct mat2
{
    float m00, m01, m10, m11;
};

inline mat2
new_mat2(f32 m00, f32 m01, f32 m10, f32 m11)
{
    mat2 result;
    result.m00 = m00;
    result.m01 = m01;
    result.m10 = m10;
    result.m11 = m11;
    return(result);
}

inline mat2
new_mat2(mat3 m)
{
    mat2 result;
    result.m00 = m.m00;
    result.m01 = m.m01;
    result.m10 = m.m10;
    result.m11 = m.m11;
    return(result);
}

inline mat2
mat2_submat3(mat3 matrix, int row_ignore, int col_ignore)
{
    mat2 result = {};
    f32 *r = &result.m00;
    f32 *m = &matrix.m00;
    
    int sub_index = 0;
    for(int row = 0; row < 3; ++row)
    {
        for(int col = 0; col < 3; ++col)
        {
            if((row != row_ignore) && (col != col_ignore))
            {
                r[sub_index++] = m[3 * row + col];
            }
        }
    }
    
    return(result);
}

inline f32 cofactor(mat3 m, int row, int col);
inline f32 cofactor(mat4 m, int row, int col);

inline f32
det(mat2 m)
{
    f32 result;
    result = m.m00 * m.m11 - m.m01 * m.m10;
    return(result);
}

inline f32
det(mat3 m)
{
    f32 result = 0;
    result += m.m00 * cofactor(m, 0, 0);
    result += m.m01 * cofactor(m, 0, 1);
    result += m.m02 * cofactor(m, 0, 2);
    return(result);
}

inline f32
det(mat4 m)
{
    f32 result = 0;
    result += m.m00 * cofactor(m, 0, 0);
    result += m.m01 * cofactor(m, 0, 1);
    result += m.m02 * cofactor(m, 0, 2);
    result += m.m03 * cofactor(m, 0, 3);
    return(result);
}

inline f32
cofactor(mat4 m, int row, int col)
{
    mat3 m_sub = mat3_submat4(m, row, col);
    f32 result = det(m_sub) * pow(-1, row + col);
    return(result);
}

inline f32
cofactor(mat3 m, int row, int col)
{
    mat2 m_sub = mat2_submat3(m, row, col);
    f32 result = det(m_sub) * pow(-1, row + col);
    return(result);
}

inline f32
minor(mat3 m, int minor_row, int minor_col)
{
    mat2 m_sub = mat2_submat3(m, minor_row, minor_col);
    f32 result = det(m_sub);
    return(result);
}

inline mat3
mat3_inverse(mat3 m)
{
    f32 d = det(m);
    Assert(d);
    
    mat3 result;
    f32 *r = &result.m00;
    
    f32 c;
    for(int row = 0; row < 3; ++row)
    {
        for(int col = 0; col < 3; ++col)
        {
            c = cofactor(m, row, col);
            r[row * 4 + col] = c / d;
        }
    }
    
    return(result);
}

inline mat4
mat4_inverse(mat4 m)
{
    f32 d = det(m);
    Assert(d);
    
    mat4 result;
    f32 *r = &result.m00;
    
    f32 c;
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            c = cofactor(m, row, col);
            r[row + col * 4] = c / d;
        }
    }
    
    return(result);
}

inline f32
radians(f32 degrees)
{
    f32 result = (degrees / 180.0f) * M_PI;
    return(result);
}

inline f32
fmin(f32 a, f32 b)
{
    return a < b ? a : b;
}

inline f32
fmax(f32 a, f32 b)
{
    return a > b ? a : b;
}

inline mat4
ortho_matrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
{
    mat4 result = mat4_identity();
    result.m00 = 2 / (right - left);
    result.m11 = 2 / (top - bottom);
    result.m22 = -2 / (far - near);
    result.m30 = -(right + left) / (right - left);
    result.m31 = -(top + bottom) / (top - bottom);
    result.m32 = -(far + near) / (far - near);
    return result;
}

#endif //MATH_FUNCTIONS_H
