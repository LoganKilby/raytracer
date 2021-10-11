internal void
scene0(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(200, 200);
    clear(*buffer, {0.0f, 0.0f, 0.0f, 1.0f});
    
    view_plane vp;
    vp.pixel_size = 1.0f;
    set_gamma(&vp, 1.0f);
    
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

internal void
scene1(pixel_buffer *buffer)
{
    *buffer = new_pixel_buffer(400, 400);
    clear(*buffer, {0, 0, 0, 1});
    
    view_plane vp;
    vp.pixel_size = 1;
    set_gamma(&vp, 1);
    
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
