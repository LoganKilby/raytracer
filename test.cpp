#include "types.h"
#include "macros.h"
#include "math_lib.h"
#include "stdio.h"
#include "test.h"
#include "ray.cpp"
#include "shading.cpp"

int main()
{
    v2 ray_screen = ray_screen_coordinates(50, 50, 50, 50);
    printf("%d tests evaluated.\n", tests_ran);
}