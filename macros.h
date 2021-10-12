/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#include "stdio.h"

#define array_count(x) (sizeof(x) / sizeof(x[0]))
#define push_struct(array, iterrator, value) array[(*iterrator)++] = *value;

#ifdef RAYTRACER_SLOW
#define AssertBreak *(int *)0 = 0;
#define Assert(expression) { if(!(expression)) { AssertBreak; } }
#else
#define AssertBreak
#define Assert(expression);
#endif //RAYTRACER_SLOW

#endif //MACROS_H
