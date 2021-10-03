/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#ifdef RAYTRACER_SLOW
#define AssertBreak *(int *)0 = 0;
#define Assert(expression) { if(!(expression)) { AssertBreak; } }
#define AssertZero(expression) { if((expression != 0)) { AssertBreak; } };
#define FloatEqual(x, y) { return (abs(x - y) < FLT_EPSILON) ? true : false; }

#else

#define AssertBreak
#define Assert(expression);
#define AssertZero(expression);
#define FloatEqual(x, y);
#endif //RAYTRACER_SLOW

#endif //MACROS_H
