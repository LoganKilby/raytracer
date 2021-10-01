#include "macros.h"
#include "math.h"

struct v4
{
    float x, y, z, w;
};

typedef float real32;

inline v4
vector(real32 x, real32 y, real32 z)
{ 
    v4 result; 
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 0;
    return(result); 
};

inline v4
point(real32 x, real32 y, real32 z)
{
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 1;
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
v4_smul(v4 v, real32 a)
{
    v4 result;
    result.x = v.x * a;
    result.y = v.y * a;
    result.z = v.z * a;
    result.w = v.w * a;
    return(result);
}

inline v4
v4_sdiv(v4 v, real32 a)
{
    float i = 1 / a;
    
    v4 result;
    result.x = v.x * i;
    result.y = v.y * i;
    result.z = v.z * i;
    result.w = v.w * i;
    return(result);
}

inline real32
v4_dot(v4 u, v4 v)
{
    AssertZero(u.w);
    AssertZero(v.w);
    real32 result;
    result = u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
    return(result);
}

inline real32
v4_magnitude(v4 v)
{
    AssertZero(v.w);
    real32 result;
    result = sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
    return(result);
}

inline v4
v4_normalize(v4 v)
{
    AssertZero(v.w);
    float inverse_magnitude = 1 / sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    
    v4 result;
    result.x = v.x * inverse_magnitude;
    result.y = v.y * inverse_magnitude;
    result.z = v.z * inverse_magnitude;
    result.w = 0;
    return(result);
}

int main()
{
    return(0);
}