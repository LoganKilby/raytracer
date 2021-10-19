internal void
scene2_hmh(pixel_buffer *buffer)
{
    f32 percent_progress = 0;
    TIMED_BLOCK;
    
    *buffer = allocate_pixel_buffer(1280, 720);
    
    material materials[7] = {};
    materials[0].emit_color = V3(0.3f, 0.4f, 0.5f);
    materials[1].reflect_color = V3(0.5f, 0.5f, 0.5f);
    materials[2].reflect_color = V3(0.7f, 0.5f, 0.3f);
    materials[3].emit_color = V3(5.0f, 0.0f, 0.0f);
    materials[4].reflect_color = V3(0.2f, 0.8f, 0.2f);
    materials[4].scatter = 0.75f;
    materials[5].reflect_color = V3(0.4f, 0.8f, 0.9f);
    materials[5].scatter = 0.85f;
    materials[6].reflect_color = V3(0.95f, 0.95f, 0.95f);
    materials[6].scatter = 1.0f;
    
    plane planes[1] = {};
    planes[0].d = 0;
    planes[0].normal = V3(0, 0, 1);
    planes[0].material_index = 1;
    
    sphere spheres[5] = {};
    spheres[0].origin = V3(0, 0, 0);
    spheres[0].radius = 1;
    spheres[0].material_index = 2;
    spheres[1].origin = V3(3, -2, 0);
    spheres[1].radius = 1;
    spheres[1].material_index = 3;
    spheres[2].origin = V3(-2, -1, 2);
    spheres[2].radius = 1.0;
    spheres[2].material_index = 4;
    spheres[3].origin = V3(1, -1, 3);
    spheres[3].radius = 1.0;
    spheres[3].material_index = 5;
    spheres[4].origin = V3(2.5, 3, 0);
    spheres[4].radius = 2.0;
    spheres[4].material_index = 6;
    
    world world = {};
    world.material_count = array_count(materials);
    world.materials = materials;
    world.plane_count = array_count(planes);
    world.planes = planes;
    world.sphere_count = array_count(spheres);
    world.spheres = spheres;
    
    camera camera = {};
    camera.position = V3(0, -10, 1);
    camera.z_axis = normalize(camera.position);
    camera.x_axis = normalize(outer(V3(0, 0, 1), camera.z_axis));
    camera.y_axis = normalize(outer(camera.z_axis, camera.x_axis));
    
    f32 film_distance = 1.0;
    f32 film_width = 1.0;
    f32 film_height = 1.0;
    
    if(buffer->width > buffer->height)
    {
        film_height = film_width * (f32)buffer->height / (f32)buffer->width;
    }
    else if(buffer->height > buffer->width)
    {
        film_width = film_height * (f32)buffer->width / (f32)buffer->height;
    }
    
    f32 half_film_width = 0.5f * film_width;
    f32 half_film_height = 0.5f * film_height;
    v3 film_center = camera.position - film_distance * camera.z_axis;
    
    f32 pixel_width = 0.5f / buffer->width;
    f32 pixel_height = 0.5f / buffer->height;
    
    ray r;
    
    for(int row = 0; row < buffer->height; ++row)
    {
        f32 film_y = -1.0 + 2.0 * ((f32)row / (f32)buffer->height);
        for(int col = 0; col < buffer->width; ++col)
        {
            f32 film_x = -1.0 + 2.0 * ((f32)col / (f32)buffer->width);
            
            v3 color = {};
            u32 rays_per_pixel = 8;
            f32 color_contribution = 1.0f / (f32)rays_per_pixel;
            for(u32 ray_index = 0; ray_index < rays_per_pixel; ++ray_index)
            {
                f32 offset_x = film_x + random_bilateral() * pixel_width;
                f32 offset_y = film_y + random_bilateral() * pixel_height;
                
                v3 film_position = film_center + (camera.x_axis * offset_x * half_film_width) + (camera.y_axis * offset_y * half_film_height);
                
                r.origin = camera.position;
                r.direction = normalize(film_position - camera.position);
                
                color += color_contribution * ray_cast(&world, r.origin, r.direction);
            }
            
            set_pixel(buffer, col, row, V4(color, 1.0));
        }
        
        percent_progress = ((f32)row / (f32)buffer->height) * 100;
        printf("\rrendering... %.0f%%", percent_progress);
        fflush(stdout);
    }
    
    printf("\n");
}
