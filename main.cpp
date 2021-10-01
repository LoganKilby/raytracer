#include "macros.h"

typedef struct v4
{
    float x, y, z, w;
} point, vector;

struct v3
{
    float x, y, z;
};

typedef float real32;

inline v4
set_vector(real32 x, real32 y, real32 z)
{ 
    v4 result; 
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 0;
    return(result); 
};

inline v4
set_point(real32 x, real32 y, real32 z)
{
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = 1;
    return(result);
}

inline v4
v4_add(v4 s, v4 t)
{
    v4 result;
    result.x = s.x + t.x;
    result.y = s.y + t.y;
    result.z = s.z + t.z;
    result.w = s.w + t.w;
    return(result);
}

inline v4
v4_sub(v4 s, v4 t)
{
    v4 result;
    result.x = s.x - t.x;
    result.y = s.y - t.y;
    result.z = s.z - t.z;
    result.w = s.w - t.w;
    return(result);
}

int main()
{
    
    return(0);
}