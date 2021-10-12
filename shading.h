/* date = October 6th 2021 10:17 am */

#ifndef SHADING_H
#define SHADING_H

struct point_light
{
    v3 intensity;
    v3 position;
};

struct material
{
    v4 color;
    f32 ambient;
    f32 diffuse;
    f32 specular;
    f32 shininess;
};

struct v2_array
{
    v2 *data;
    int count; // num_samples per pattern
    int max_count;
};

struct u32_array
{
    u32 *data;
    int count;
    int max_count;
};

struct pixel_sampler
{
    // generate samples ()
    // setup shuffled indices ()
    // shuffle samples ()
    
    // NOTE: num_samples > 1 ? "multi-jittered" : "regular" anti-aliasing
    int jump; // random index jump
    // NOTE: multi-jitter requires a perfect square # of samples (I think)
    int num_samples; // 16, 25
    int num_sets; // 83
    // NOTE: the amount of space for samples is num_sets * num_samples
    v2 *samples; // sample points on a unit square
    int *shuffled_indices; // shuffled samples array indices
    unsigned long count; // the current number of sample points used
};

inline material default_material();

#endif //SHADING_H
