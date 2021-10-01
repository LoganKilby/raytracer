/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#ifdef RAYTRACER_SLOW
#define AssertBreak *(int *)0 = 0;
#define Assert(expression) { if(!(expression)) { AssertBreak; } }
#define FloatEqual(x, y) { return (abs(x - y) < FLT_EPSILON) ? true : false; }
#define AssertZero(expression) { if((expression)) { AssertBreak; } };

#else

#define AssertBreak;
#define Assert(expression);
#define FloatEqual(x, y);
#define AssertZero(expression);
#endif //RAYTRACER_SLOW

#endif //MACROS_H
