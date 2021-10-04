/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#include "stdio.h"

#ifdef RAYTRACER_SLOW
#define AssertBreak *(int *)0 = 0;
#define Assert(expression) { if(!(expression)) { printf("assertion triggered. exiting...\n"); AssertBreak; } }
#define AssertZero(expression) { if((expression != 0)) { AssertBreak; } };
#define FloatEqual(x, y) (fabs(x - y) < FLT_EPSILON)
#define FloatZero(x) fabs(x) < FLT_EPSILON

#else

#define AssertBreak
#define Assert(expression);
#define AssertZero(expression);
#define FloatEqual(x, y);

#endif //RAYTRACER_SLOW
#endif //MACROS_H
