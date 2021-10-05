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

global_variable s32 tests_ran = 0;
#define TrackResult(Status) { test t(__COUNTER__, __func__, __LINE__, Status); tests_ran++;}

bool mat4_equality(glm::mat4 a, glm::mat4 b)
{
    bool result = 1;
    for(int row = 0; row < 4; ++row)
    {
        for(int col = 0; col < 4; ++col)
        {
            result &= (a[row][col] == b[row][col]);
        }
    }
    
    return result;
}

bool v4_equality(glm::vec4 v, glm::vec4 u)
{
    f32 fe2 = FLT_EPSILON;
    f32 x = fabs(v.x - u.x);
    f32 y = fabs(v.y - u.y);
    f32 z = fabs(v.z - u.z);
    f32 w = fabs(v.w - u.w);
    
    bool result_x = x < FLT_EPSILON;
    bool result_y = y < FLT_EPSILON;
    bool result_z = z < FLT_EPSILON;
    bool result_w = w < FLT_EPSILON;
    
    
    return(result_x && result_y && result_z && result_w);
}

bool mat3_equality(glm::mat3 mat_a, glm::mat3 mat_b)
{
    f32 *a = &mat_a[0][0];
    f32 *b = &mat_b[0][0];
    
    bool result = 1;
    for(int row = 0; row < 3; ++row)
    {
        for(int col = 0; col < 3; ++col)
        {
            result &= (a[row * 3 + col] == b[row * 3 + col]);
        }
    }
    
    return result;
}

#endif //TEST_H
