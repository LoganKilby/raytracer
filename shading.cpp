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

internal void
generate_shuffled_indices(pixel_sampler *sampler)
{
    if(sampler->num_samples = 1)
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
        u32 *indices = (u32 *)malloc(sizeof(u32) * samples_per_set);
        
        for(int sample_index = 0; sample_index < samples_per_set; ++sample_index)
        {
            indices[sample_index] = sample_index;
        }
        
        for(int set_index = 0; set_index < total_sets; ++set_index)
        {
            u32_array_shuffle(indices, samples_per_set);
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
    //Assert(sampler->num_samples == 1);
    
    for(int set_index = 0; set_index < sampler->num_sets; ++set_index)
    {
        sampler->samples[set_index].x = f32rand();
        sampler->samples[set_index].y = f32rand();
    }
}

internal void
generate_multi_jitter_samples(pixel_sampler *sampler)
{
    int n = (int)sqrt(sampler->num_samples);
    
    
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
create_pixel_sampler(int num_samples, int num_sets)
{
    pixel_sampler result = {};
    result.num_sets = num_sets;
    result.num_samples = num_samples;
    // TODO: Arena?
    result.samples = (v2 *)malloc(sizeof(v2) * num_sets * num_samples);
    result.shuffled_indices = (int *)malloc(sizeof(int) * num_sets * num_samples);
    
    if(num_samples == 1)
    {
        generate_noise_samples(&result);
        generate_shuffled_indices(&result);
    }
    else
    {
        generate_multi_jitter_samples(&result);
        //v2_array_shuffle_x(result.samples, result.num_samples * result.num_sets);
        //v2_array_shuffle_y(result.samples, result.num_samples * result.num_sets);
        generate_shuffled_indices(&result);
        // TODO: I still don't understand entirely if I really need to have shuffled indices with multi
        // jitter since I'm already shuffling the x and y values
        // TODO: Check out correlated multi-jitter sampling: https://graphics.pixar.com/library/MultiJitteredSampling/paper.pdf
    }
    
    return result;
}
