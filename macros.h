/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#ifdef RAYTRACER_SLOW
#define Assert(expression) { if(!(expression)) { *(int *)0 = 0; } }
#define FloatEqual(x, y) { return (abs(x - y) < FLT_EPSILON) ? true : false; }

#else

#define Assert(expression);
#define FloatEqual(x, y);
#endif //RAYTRACER_SLOW

#endif //MACROS_H
