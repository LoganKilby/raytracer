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
            if(ray_sphere_intersection(r, s, &hit))
            {
                set_pixel(*buffer, col, row, pixel_color);
            }
            
        }
    }
}
