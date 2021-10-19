internal void
scene2_hmh(pixel_buffer buffer, u32 *total_bounces)
{
    f32 percent_progress = 0;
    
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
    
    u32 core_count = 4; // TODO: Get user core count
    u32 tile_width = buffer.width / core_count;
    u32 tile_height = tile_width;
    printf("core_count %d with %dx%d (%dk/tile) tiles\n",core_count, tile_width, tile_height, tile_width * tile_height * 4 * 4 / 1024);
    
    u32 tile_count_x = (buffer.width + tile_width - 1) / tile_width;
    u32 tile_count_y = (buffer.height + tile_height - 1) / tile_height;
    u32 total_tile_count = tile_count_x * tile_count_y;
    u32 tiles_retired = 0;
    
    for(u32 tile_y = 0; tile_y < tile_count_y; ++tile_y)
    {
        u32 min_y = tile_y * tile_height;
        u32 max_y = min_y + tile_height;
        if(max_y > buffer.height)
        {
            max_y = buffer.height;
        }
        
        for(u32 tile_x = 0; tile_x < tile_count_x; ++tile_x)
        {
            u32 min_x = tile_x * tile_width;
            u32 max_x = min_x + tile_width;
            if(max_x > buffer.width)
            {
                max_x = buffer.width;
            }
            
            render_tile(&world, &buffer, min_x, min_y, max_x, max_y);
            ++tiles_retired;
            printf("\rrendering... %.0f%%", ((f32)tiles_retired / (f32)total_tile_count) * 100);
            fflush(stdout);
        }
    }
    
    printf("\n");
    *total_bounces = world.total_bounces;
}
