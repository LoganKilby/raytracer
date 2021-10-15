/* date = October 1st 2021 1:50 pm */

#ifndef MACROS_H
#define MACROS_H

#define array_count(x) (sizeof(x) / sizeof(x[0]))
#define push_struct(array, iterrator, value) array[(*iterrator)++] = *value;


#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "stdio.h"

struct block_timer
{
    const char* function_name;
    LARGE_INTEGER timer_start;
    LARGE_INTEGER frequency;
	
	block_timer(const char *caller)
	{
        function_name = caller;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&timer_start);
    }
    
	~block_timer()
	{
		LARGE_INTEGER timer_end;
        QueryPerformanceCounter(&timer_end);
        timer_end.QuadPart = timer_end.QuadPart - timer_start.QuadPart;
        timer_end.QuadPart *= 1000000;
        timer_end.QuadPart /= frequency.QuadPart;
        
        fprintf(stderr, "%s: %.3Lf ms\n", function_name, timer_end.QuadPart / 1000.0l);
    }
};

#ifdef RAYTRACER_SLOW
#define AssertBreak *(int *)0 = 0;
#define Assert(expression) { if(!(expression)) { AssertBreak; } }
#define TIMED_BLOCK block_timer timer##__LINE__(__FUNCTION__);
#else
#define AssertBreak
#define Assert(expression);
#define TIMED_BLOCK
#endif //RAYTRACER_SLOW



#endif //MACROS_H
