#include "shading.h"

using namespace glm;

internal v4
calc_point_light(material material, point_light light, v3 view_point, v3 eyev, v3 surface_normal)
{
    v3 normal = v3(surface_normal);
    v3 eye_vector = v3(eyev);
    
    v3 effective_color = v3(material.color) * light.intensity;
    v3 ambient_component = effective_color * material.ambient;
    
    v3 light_direction = glm::normalize(light.position - view_point);
    f32 light_dot_normal = glm::max(glm::dot(light_direction, normal), 0.0f);
    v3 diffuse_component = light_dot_normal * material.diffuse * effective_color;
    
    v3 reflect_dir = glm::reflect(-light_direction, normal);
    f32 spec_factor = glm::pow(glm::max(glm::dot(reflect_dir, eye_vector), 0.0f), material.shininess);
    v3 specular_component = light.intensity * spec_factor * material.specular;
    
    v4 result = v4(v3(ambient_component + diffuse_component + specular_component), 1); 
    
    return result;
}

inline v3
eye_ray_direction(int row, int col, view_plane vp)
{
    return glm::normalize(v3(vp.pixel_size * (col - vp.width / 2 + 0.5),
                             vp.pixel_size * (row - vp.height / 2 + 0.5),
                             -vp.distance));
}

inline material
default_material()
{
    material m;
    m.color = v4(1, 1, 1, 0);
    m.ambient = 0.1f;
    m.diffuse = 0.9f;
    m.specular = 0.9f;
    m.shininess = 200.0f;
    return m;
}

internal void
u32_array_shuffle(u32 *data, int count)
{
    int j = count - 1;
    
    f32 u;
    u32 temp;
    int k;
    for(;;)
    {
        u = f32rand();
        k = (j * u) + 1;
        temp = data[k];
        data[k] = data[j];
        data[j] = temp;
        if(j > 0)
        {
            --j;
        }
        else
        {
            break;
        }
    }
}

internal void
int_array_shuffle(int *data, int count)
{
    int j = count - 1;
    
    f32 u;
    u32 temp;
    int k;
    for(;;)
    {
        u = f32rand();
        k = (j * u) + 1;
        temp = data[k];
        data[k] = data[j];
        data[j] = temp;
        if(j > 0)
        {
            --j;
        }
        else
        {
            break;
        }
    }
}

internal void
v2_array_shuffle_x(v2 *data, int count)
{
    int j = count - 1;
    
    f32 u;
    f32 temp;
    int k;
    for(;;)
    {
        u = f32rand();
        k = (j * u) + 1;
        temp = data[k].x;
        data[k].x = data[j].x;
        data[j].x = temp;
        if(j > 0)
        {
            --j;
        }
        else
        {
            break;
        }
    }
}

internal void
v2_array_shuffle_y(v2 *data, int count)
{
    int j = count - 1;
    
    f32 u;
    f32 temp;
    int k;
    for(;;)
    {
        u = f32rand();
        k = (j * u) + 1;
        temp = data[k].y;
        data[k].y = data[j].y;
        data[j].y = temp;
        if(j > 0)
        {
            --j;
        }
        else
        {
            break;
        }
    }
}

inline v2
sample_unit_square(pixel_sampler *sampler)
{
    if(sampler->count % sampler->num_samples == 0)
        sampler->jump = (rand_int() % sampler->num_sets) * sampler->num_samples;
    
    return sampler->samples[sampler->jump + sampler->shuffled_indices[sampler->jump + sampler->count++ % sampler->num_samples]];
}

inline v2
sample_unit_disk(pixel_sampler *sampler)
{
    if(sampler->count % sampler->num_samples == 0)
        sampler->jump = (rand_int() % sampler->num_sets) * sampler->num_samples;
    
    return sampler->disk_samples[sampler->jump + sampler->shuffled_indices[sampler->jump + sampler->count++ % sampler->num_samples]];
}

internal void
generate_shuffled_indices(pixel_sampler *sampler)
{
    if(sampler->num_samples == 1)
    {
        for(int i = 0; i < sampler->num_sets; ++i)
        {
            sampler->shuffled_indices[i] = i;
        }
        
        int_array_shuffle(sampler->shuffled_indices, sampler->num_sets);
    }
    else
    {
        int total_sets = sampler->num_sets;
        int samples_per_set = sampler->num_samples;
        // TODO: Arena, transient memory
        int *indices = (int *)malloc(sizeof(int) * samples_per_set);
        
        for(int sample_index = 0; sample_index < samples_per_set; ++sample_index)
        {
            indices[sample_index] = sample_index;
        }
        
        for(int set_index = 0; set_index < total_sets; ++set_index)
        {
            int_array_shuffle(indices, samples_per_set);
            for(int sample_index = 0; sample_index < samples_per_set; ++sample_index)
            {
                sampler->shuffled_indices[set_index * samples_per_set + sample_index] = indices[sample_index];
            }
        }
        
        free(indices);
    }
}

internal void
generate_jittered_samples(pixel_sampler *sampler)
{
    int n = (int)sqrt(sampler->num_samples);
    
    int sets = sampler->num_sets;
    int current_index = 0;
    for(int p = 0; p < sets; ++p)
    {
        for(int j = 0; j < n; ++j)
        {
            for(int k = 0; k < n; ++k)
            {
                v2 p = v2((k + f32rand()) / n, (j + f32rand()) / n);
                sampler->samples[current_index++] = p;
            }
        }
    }
}

internal void
generate_noise_samples(pixel_sampler *sampler)
{
    for(int set_index = 0; set_index < sampler->num_sets; ++set_index)
    {
        for(int sample_index = 0; sample_index < sampler->num_samples; ++sample_index)
        {
            sampler->samples[set_index * sampler->num_samples + sample_index].x = f32rand();
            sampler->samples[set_index * sampler->num_samples + sample_index].y = f32rand();
        }
    }
}

internal void
generate_nrooks_samples(pixel_sampler *sampler)
{
    int num_sets = sampler->num_sets;
    int num_samples = sampler->num_samples;
    int total_samples = num_sets * num_samples;
    
    v2 sample;
    for (int p = 0; p < num_sets; p++)
    {
        for (int j = 0; j < num_samples; j++)
        {
            sample.x = (j + f32rand()) / num_samples;
            sample.y = (j + f32rand()) / num_samples;
            sampler->samples[p * num_samples + j] = sample;
        } 
    }
    
    v2_array_shuffle_x(sampler->samples, total_samples);
    v2_array_shuffle_y(sampler->samples, total_samples); 
}

internal void
generate_multi_jitter_samples(pixel_sampler *sampler)
{
    int n = (int)sqrt(sampler->num_samples);
    
    // TODO: using the sqrt(n) * sqrt(n) grid to jitter samples
    // map these on a n * n grid
    // ... FINISH MULTI JITTER
    
    for(int set_index = 0; set_index < sampler->num_sets; ++set_index)
    {
        for(int j = 0; j < n; ++j)
        {
            for(int i = 0; i < n; ++i)
            {
                sampler->samples[j * n + i].x = (i + (j + f32rand()) / n) / n;
                sampler->samples[j * n + i].y = (j + (i + f32rand()) / n) / n;
            }
        }
    }
}

internal pixel_sampler
jitter_sampler(int num_samples, int num_sets)
{
    pixel_sampler result = {};
    result.num_sets = num_sets;
    result.num_samples = num_samples;
    // TODO: Arena?
    result.samples = (v2 *)malloc(sizeof(v2) * num_sets * num_samples);
    result.shuffled_indices = (int *)malloc(sizeof(int) * num_sets * num_samples);
    generate_multi_jitter_samples(&result);
    //v2_array_shuffle_x(result.samples, result.num_samples * result.num_sets);
    //v2_array_shuffle_y(result.samples, result.num_samples * result.num_sets);
    generate_shuffled_indices(&result);
    // TODO: I still don't understand entirely if I really need to have shuffled indices with multi
    // jitter since I'm already shuffling the x and y values
    // TODO: Check out correlated multi-jitter sampling: https://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf
    return result;
}

internal pixel_sampler
noise_sampler(int num_samples, int num_sets)
{
    pixel_sampler result = {};
    result.num_sets = num_sets;
    result.num_samples = num_samples;
    // TODO: Arena?
    result.samples = (v2 *)malloc(sizeof(v2) * num_sets * num_samples);
    result.shuffled_indices = (int *)malloc(sizeof(int) * num_sets * num_samples);
    
    generate_noise_samples(&result);
    generate_shuffled_indices(&result);
    
    return result;
}

internal pixel_sampler
nrooks_sampler(int num_samples, int num_sets)
{
    pixel_sampler result = {};
    result.num_sets = num_sets;
    result.num_samples = num_samples;
    // TODO: Arena?
    result.samples = (v2 *)malloc(sizeof(v2) * num_sets * num_samples);
    result.shuffled_indices = (int *)malloc(sizeof(int) * num_sets * num_samples);
    
    generate_nrooks_samples(&result);
    generate_shuffled_indices(&result);
    
    return result;
}

internal void
map_samples_to_unit_disk(pixel_sampler *sampler)
{
    // quarter 1: x > y; x > -y; theta [-PI/4, PI/4]; r = x, phi = (PI/4)(y/x)
    // quarter 2: x < y; x > -y; theta [PI/4, 3PI/4]; r = y, phi = (PI/4)(4 + y/x)
    // quarter 3: x < y; x < -y; theta [3PI/4, 5PI/4]; r = -x; phi = (PI/4)(4 + y/x)
    // quarter 4: x > y; x < -y; theta [5PI/4, 7PI/4]; r = -y; phi = (PI/4)(6 - x/y)
    
    int total_samples = sampler->num_sets * sampler->num_samples;
    f32 r, phi;
    v2 sample_point;
    
    for(int j = 0; j < total_samples; ++j)
    {
        // map sample point to [-1, 1] [-1, 1]
        sample_point.x = 2.0 * sampler->samples[j].x - 1.0;
        sample_point.y = 2.0 * sampler->samples[j].y - 1.0;
        
        // x > y; x > -y
        if(sample_point.x > -sample_point.y)
        {
            if(sample_point.x > sample_point.y)
            {
                // quarter 1
                r = sample_point.x;
                phi = sample_point.y / sample_point.x;
            }
            else
            {
                // quarter 2
                r = sample_point.y;
                phi = 2 - sample_point.x / sample_point.y;
            }
        }
        else
        {
            if(sample_point.x < sample_point.y)
            {
                r = -sample_point.x;
                phi = 4 + sample_point.y / sample_point.x;
            }
            else
            {
                r = -sample_point.y;
                // catch div by 0
                if(sample_point.y != 0)
                {
                    phi = 6 - sample_point.x / sample_point.y;
                }
                else
                {
                    phi = 0;
                }
            }
        }
        
        phi *= PI / 4;
        sampler->disk_samples[j].x = r * cos(phi);
        sampler->disk_samples[j].y = r * sin(phi);
    }
}

internal void
map_samples_to_unit_hemisphere(pixel_sampler *sampler, f32 e)
{
    // NOTE: e defines how rapidly the surface density of the samples 
    // on the unit hemisphere will decrease as theta increases.
    // The author uses e=1 or e=0
    
    // Shirley and Morley (2003)
    
    // given two random numbers [r1, r2];
    // (azimuth, phi) = [2PI * r1, acos((1 - r2) ^ (1 / e + 1))]
    
    // a sample point p on the hemisphere is defined as:
    // p = sin(theta) * cos(phi) * u + sin(theta) * sin(phi) * v + cos(theta) * w
    int total_samples = sampler->num_sets * sampler->num_samples;
    
    f32 cos_phi, sin_phi, cos_theta, sin_theta;
    f32 u, v, w;
    for(int i = 0; i < total_samples; ++i)
    {
        cos_phi = cos(2.0 * PI * sampler->samples[i].x);
        sin_phi = sin(2.0 * PI * sampler->samples[i].x);
        cos_theta = pow(1 - sampler->samples[i].y, 1 / (e + 1));
        sin_theta = sqrt(1 - cos_theta * cos_theta);
        u = sin_theta * cos_phi;
        v = sin_theta * sin_phi;
        w = cos_theta;
        
        sampler->hemisphere_samples[i] = v3(u, v, w);
    }
}