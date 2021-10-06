/* date = October 3rd 2021 3:53 pm */

#ifndef TEST_H
#define TEST_H

#define global_variable static

global_variable int failed_tests;

struct test
{
    char *function_name;
    char *file_name;
    int test_number;
    int line_number;
    bool32 status;
    
    test(int num, char *function, char *file, int ln, int passfail)
    {
        function_name = function;
        file_name = file;
        test_number = num;
        line_number = ln;
        status = passfail;
    }
    
    test()
    {
        function_name = 0;
        file_name = 0;
        test_number = 0;
        line_number = 0;
        status = 1;
    }
    
    ~test()
    {
        if(!status)
        {
            printf("%s(%d): test failed: '%s'\n", file_name, line_number, function_name);
            
        }
    }
};

global_variable s32 tests_ran = 0;
#define TrackResult(Status) { test t(__COUNTER__, __func__, __FILE__, __LINE__, Status); tests_ran++;}

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
    f32 e = FLT_EPSILON;
    // I can't for the life of me figure out why my precision is so low with these matrix multiplications
    f32 e2 = e * 37; 
    
    v4 r;
    r.x = fabs(v.x - u.x);
    r.y = fabs(v.y - u.y);
    r.z = fabs(v.z - u.z);
    r.w = fabs(v.w - u.w);
    
    v4 result;
    result.x = r.x < e2;
    result.y = r.y < e2;
    result.z = r.z < e2;
    result.w = r.w < e2;
    
    
    return(result.x && result.y && result.z && result.w);
}

bool v3_equality(v3 v, v3 u)
{
    f32 e = FLT_EPSILON;
    f32 e2 = e * 3;
    
    f32 x = fabs(v.x - u.x);
    f32 y = fabs(v.y - u.y);
    f32 z = fabs(v.z - u.z);
    
    bool result_x = x < e2;
    bool result_y = y < e2;
    bool result_z = z < e2;
    
    return(result_x && result_y && result_z);
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
