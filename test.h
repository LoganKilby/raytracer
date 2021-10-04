/* date = October 3rd 2021 3:53 pm */

#ifndef TEST_H
#define TEST_H

#define global_variable static

global_variable int failed_tests;

struct test
{
    char *function_name;
    int test_number;
    int line_number;
    bool32 status;
    
    test(int num, char *function, int ln, int passfail)
    {
        test_number = num;
        function_name = function;
        line_number = ln;
        status = passfail;
    }
    
    test()
    {
        test_number = 0;
        function_name = 0;
        line_number = 0;
        status = 1;
    }
    
    ~test()
    {
        if(!status)
        {
            printf("\n:: test failed (%d) :: \nfunction: \"%s\"\nline: %d\n\n",
                   test_number,
                   function_name,
                   line_number);
        }
    }
};

#define TrackResult(Status) { test t(__COUNTER__, __func__, __LINE__, Status); }

bool mat4_equality(mat4 mat_a, mat4 mat_b)
{
    f32 *a = &mat_a.m00;
    f32 *b = &mat_b.m00;
    
    bool result = 1;
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            result &= (a[row * 4 + col] == b[row * 4 + col]);
        }
    }
    
    return result;
}

bool v4_equality(v4 v, v4 u)
{
    f32 fe2 = FLT_EPSILON * 2.0f;
    bool x = fabs(v.x - u.x) < fe2;
    bool y = fabs(v.y - u.y) < fe2;
    bool z = fabs(v.z - u.z) < fe2;
    bool w = fabs(v.w - u.w) < fe2;
    
    return(x && y && z && w);
}

#endif //TEST_H
