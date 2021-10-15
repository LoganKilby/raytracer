// jitter sampling
internal void
scene1_jitter(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(400, 400);
    clear(*buffer, {0, 0, 0, 1});
    
    view_plane vp;
    vp.pixel_size = 1;
    
    pixel_sampler sampler = noise_sampler(16, 83);
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 1);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(0, -25, 0);
    s0.radius = 80;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(0, 30, 0);
    s1.radius = 60;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.direction = v3(0, 0, -1);
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    v4 color;
    f32 x, y, z = 100;
    v2 pixel_sample;
    
    f32 view_x, view_y;
    int width = buffer->width, height = buffer->height;
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            color = {};
            for(int samples = 0; samples < sampler.num_samples; ++samples)
            {
                pixel_sample = sample_unit_square(&sampler);
                x = vp.pixel_size * (col - 0.5 * width + pixel_sample.x);
                y = vp.pixel_size * (row - 0.5 * height + pixel_sample.y);
                r.origin = v3(x, y, z);
                ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
                ray_plane_intersections(r, &p, 1, &hit_group);
                
                if(hit_group.count)
                {
                    near_hit = nearest_hit(&hit_group);
                    color += near_hit.rgb;
                }
                
                hit_group.count = 0;
            }
            
            // Not using alpha here
            color /= sampler.num_samples;
            set_pixel(*buffer, col, row, color);
        }
    }
}

// regular aliasing
internal void
scene1_thin_lens(pixel_buffer *buffer)
{
    TIMED_BLOCK;
    
    view_plane vp;
    vp.pixel_size = 1;
    vp.width = 200;
    vp.height = 200;
    
    *buffer = new_pixel_buffer(vp.width, vp.height);
    clear(*buffer, {0, 0, 0, 1});
    
    thin_lens_camera camera = {};
    camera.position = v3(30, 3, 50);
    camera.look_at = v3(0, 0, 0);
    camera.up = v3(0, 1, 0);
    camera.vp_distance = 50;
    camera.lens_radius = 15;
    camera.focal_length = 74;
    zoom_pinhole_camera(&camera, 1);
    camera_compute_basis(&camera);
    
    pixel_sampler sampler = nrooks_sampler(256, 83);
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 0);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(-50, 15, 0);
    s0.radius = 10;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(30, 15, 0);
    s1.radius = 10;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.origin = camera.position;
    
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    v4 color;
    f32 x, y, z = 100;
    f32 view_x, view_y;
    v2 unit_square_sample;
    v2 unit_disk_sample;
    v2 pixel_coords;
    v2 lens_coords;
    
    int n = (int)sqrt((float)sampler.num_samples);
    
    int width = buffer->width, height = buffer->height;
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            color = {};
            for(int up = 0; up < n; ++up)
            {
                for(int across = 0; across < n; ++across)
                {
                    unit_square_sample = sample_unit_square(&sampler);
                    
                    pixel_coords.x = vp.pixel_size * camera.zoom * (col - 0.5 * width +  unit_square_sample.x);
                    pixel_coords.y = vp.pixel_size * camera.zoom * (row - 0.5 * height + unit_square_sample.y);
                    
                    lens_coords = sample_unit_disk(&sampler) * camera.lens_radius;
                    
                    r.origin = camera.position + lens_coords.x * camera.u + lens_coords.y * camera.v;
                    r.direction = thin_lens_ray_direction(&camera, lens_coords, pixel_coords);
                    ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
                    ray_plane_intersections(r, &p, 1, &hit_group);
                    
                    if(hit_group.count)
                    {
                        near_hit = nearest_hit(&hit_group);
                        color += near_hit.rgb;
                    }
                    
                    hit_group.count = 0;
                }
            }
            
            // Not using alpha here
            color /= sampler.num_samples;
            set_pixel(*buffer, col, row, color);
        }
    }
}

// regular aliasing
internal void
scene1_noise(pixel_buffer *buffer)
{
    TIMED_BLOCK;
    
    view_plane vp;
    vp.pixel_size = 1;
    vp.width = 200;
    vp.height = 200;
    
    *buffer = new_pixel_buffer(vp.width, vp.height);
    clear(*buffer, {0, 0, 0, 1});
    
    pinhole_camera eye_camera = {};
    eye_camera.position = v3(300, 400, 500);
    eye_camera.look_at = v3(0, -25, -50);
    eye_camera.up = v3(0, 1, 0);
    eye_camera.vp_distance = 400;
    zoom_pinhole_camera(&eye_camera, 0.5);
    camera_compute_basis(&eye_camera);
    
    //pixel_sampler sampler = noise_sampler(16, 83);
    pixel_sampler sampler = nrooks_sampler(16, 83);
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 1);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(0, -25, 0);
    s0.radius = 80;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(0, 30, 0);
    s1.radius = 60;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.origin = eye_camera.position;
    
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    v4 color;
    f32 x, y, z = 100;
    f32 view_x, view_y;
    v2 sampled_noise;
    v2 pixel_coords;
    
    int n = (int)sqrt((float)sampler.num_samples);
    
    int width = buffer->width, height = buffer->height;
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            color = {};
            for(int up = 0; up < n; ++up)
            {
                for(int across = 0; across < n; ++across)
                {
                    sampled_noise = sample_unit_square(&sampler);
                    
                    pixel_coords.x = vp.pixel_size * eye_camera.zoom * (col - 0.5 * width +  sampled_noise.x);
                    pixel_coords.y = vp.pixel_size * eye_camera.zoom * (row - 0.5 * height + sampled_noise.y);
                    r.direction = pinhole_ray_direction(&eye_camera, pixel_coords);
                    
                    ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
                    ray_plane_intersections(r, &p, 1, &hit_group);
                    
                    if(hit_group.count)
                    {
                        near_hit = nearest_hit(&hit_group);
                        color += near_hit.rgb;
                    }
                    
                    hit_group.count = 0;
                }
            }
            
            // Not using alpha here
            color /= sampler.num_samples;
            set_pixel(*buffer, col, row, color);
        }
    }
}

// regular aliasing
internal void
scene1_jitter_antialiasing(pixel_buffer *buffer)
{
    view_plane vp;
    vp.pixel_size = 1;
    vp.sample_count = 16;
    vp.width = 400;
    vp.height = 400;
    f32 vp_distance = 1;
    
    *buffer = new_pixel_buffer(vp.width, vp.height);
    clear(*buffer, {0, 0, 0, 1});
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 1);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(0, -25, 0);
    s0.radius = 80;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(0, 30, 0);
    s1.radius = 60;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.direction = v3(0, 0, -1);
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    v4 color;
    f32 x, y, z = 100;
    f32 view_x, view_y;
    int width = buffer->width, height = buffer->height;
    int n = (int)sqrt((float)vp.sample_count);
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            color = {};
            
            // sub-pixel grid
            for(int up = 0; up < n; ++up)
            {
                for(int across = 0; across < n; ++across)
                {
                    x = vp.pixel_size * (col - 0.5 * width + (across + f32rand()) / n);
                    y = vp.pixel_size * (row - 0.5 * height + (up + f32rand()) / n);
                    r.origin = v3(x, y, z);
                    r.direction = eye_ray_direction(row, col, vp, vp_distance);
                    ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
                    ray_plane_intersections(r, &p, 1, &hit_group);
                    if(hit_group.count)
                    {
                        near_hit = nearest_hit(&hit_group);
                        color += near_hit.rgb;
                    }
                    
                    
                    hit_group.count = 0;
                }
            }
            
            color /= vp.sample_count;
            // Not using alpha here
            set_pixel(*buffer, col, row, color);
        }
    }
}

// regular aliasing
internal void
scene1_regular_aliasing(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(400,  400);
    clear(*buffer, {0, 0, 0, 1});
    
    view_plane vp;
    vp.pixel_size = 1;
    vp.sample_count = 16;
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 1);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(0, -25, 0);
    s0.radius = 80;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(0, 30, 0);
    s1.radius = 60;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.direction = v3(0, 0, -1);
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    v4 color;
    f32 x, y, z = 100;
    f32 view_x, view_y;
    int width = buffer->width, height = buffer->height;
    int n = (int)sqrt((float)vp.sample_count);
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            color = {};
            
            // sub-pixel grid
            for(int up = 0; up < n; ++up)
            {
                for(int across = 0; across < n; ++across)
                {
                    x = vp.pixel_size * (col - 0.5 * width + (across + 0.5) / n);
                    y = vp.pixel_size * (row - 0.5 * height + (up + 0.5) / n);
                    
                    r.origin = v3(x, y, z);
                    ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
                    ray_plane_intersections(r, &p, 1, &hit_group);
                    if(hit_group.count)
                    {
                        near_hit = nearest_hit(&hit_group);
                        color += near_hit.rgb;
                    }
                    
                    hit_group.count = 0;
                }
            }
            
            // Not using alpha here
            color /= vp.sample_count;
            set_pixel(*buffer, col, row, color);
        }
    }
}

internal void
scene1(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(400, 400);
    clear(*buffer, {0, 0, 0, 1});
    
    view_plane vp;
    vp.pixel_size = 1;
    
    
    plane p;
    p.origin = v3(0, 0, 0);
    p.normal = v3(0, 1, 1);
    p.material.color = v4(0, 0.3, 0, 1);
    
    sphere s0;
    s0.origin = v3(0, -25, 0);
    s0.radius = 80;
    s0.material.color = v4(1, 0, 0, 0);
    
    sphere s1;
    s1.origin = v3(0, 30, 0);
    s1.radius = 60;
    s1.material.color = v4(1, 1, 0, 0);
    
    sphere world_geo[] = { s0, s1 };
    
    ray r;
    r.direction = v3(0, 0, -1);
    ray_hit near_hit;
    ray_hit hits[5];
    ray_hit_group hit_group;
    hit_group.max_count = array_count(hits);
    hit_group.hits = &hits[0];
    hit_group.count = 0;
    
    f32 x, y, z = 100;
    f32 view_x, view_y;
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            x = vp.pixel_size * (col - 0.5 * (buffer->width - 1));
            y = vp.pixel_size * (row - 0.5 * (buffer->height - 1));
            r.origin = v3(x, y, z);
            
            ray_sphere_intersections(r, &world_geo[0], array_count(world_geo), &hit_group);
            ray_plane_intersections(r, &p, 1, &hit_group);
            if(hit_group.count)
            {
                near_hit = nearest_hit(&hit_group);
                set_pixel(*buffer, col, row, near_hit.rgb);
            }
            
            hit_group.count = 0;
        }
    }
}

internal void
scene0(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(200, 200);
    clear(*buffer, {0.0f, 0.0f, 0.0f, 1.0f});
    
    view_plane vp;
    vp.pixel_size = 1.0f;
    
    sphere s;
    s.origin = v3(0.0f);
    s.radius = 85.0f;
    
    // render
    ray r;
    r.direction = v3(0, 0, -1);
    
    v4 pixel_color = v4(1.0f, 0.0f, 0.0f, 1.0f);
    ray_hit hit;
    
    f32 x, y, z = 100.0f;
    for(int row = 0; row < buffer->height; ++row)
    {
        for(int col = 0; col < buffer->width; ++col)
        {
            x = vp.pixel_size * (col - 0.5 * (buffer->width - 1.0f));
            y = vp.pixel_size * (row - 0.5 * (buffer->height - 1.0f));
            r.origin = v3(x, y, z);
            
            pixel_color = { cos(col), sin(row), 0.0f,  1.0f };
            
            if(ray_sphere_intersection(r, s, &hit))
            {
                set_pixel(*buffer, col, row, pixel_color);
            }
            
        }
    }
}
