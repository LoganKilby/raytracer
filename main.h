/* date = October 8th 2021 1:35 pm */

#ifndef MAIN_H
#define MAIN_H

struct pixel_buffer
{
    // NOTE: 4 floats per pixel
    f32 *data;
    u32 width;
    u32 height;
    u32 pitch;
};

internal void write_ppm(f32 *data, int width, int height, f32 gamma, char *fname);
internal pixel_buffer allocate_pixel_buffer(u32 width, u32 height);
internal void set_pixel(pixel_buffer *buffer, u32 x, u32 y, v3 color);

#define FAST_OBJ_IMPLEMENTATION
#include "include/fast_obj.h"

#define CHECK(expression)

internal void
mesh_face_lookup(fastObjMesh *mesh, u32 face_index, u32 group_index)
{
    u32 group_offset = mesh->groups[group_index].face_offset;
    u32 vertices_per_face = mesh->face_vertices[group_offset + face_index];
    
    // &mesh->indices[group_offset + face_index] is a pointer to three
    // indices of the vertices, normals, and texture coordinates making up the face
    // of a triangle in the mesh
    
    for(u32 i = 0; i < vertices_per_face; ++i)
    {
        fastObjIndex face = mesh->indices[group_offset + face_index + i];
        printf("vertices: %f %f %f\n", 
               mesh->positions[3 * face.p + 0],
               mesh->positions[3 * face.p + 1],
               mesh->positions[3 * face.p + 2]);
    }
    
}

internal void
print_face_vertices(fastObjMesh *m)
{
    for (unsigned int ii = 0; ii < m->group_count; ii++)
    {
        const fastObjGroup& grp = m->groups[ii];
        
        printf("%s\n", grp.name);
        
        CHECK(shp.name == grp_name);
        CHECK(shp.mesh.num_face_vertices.size() == grp.face_count);
        
        int idx = 0;
        for (unsigned int jj = 0; jj < grp.face_count; jj++)
        {
            unsigned int fv = m->face_vertices[grp.face_offset + jj];
            
            CHECK(shp.mesh.num_face_vertices[jj] == fv);
            
            for (unsigned int kk = 0; kk < fv; kk++)
            {
                fastObjIndex mi = m->indices[grp.index_offset + idx];
                
                CHECK(oi.vertex_index + 1 == mi.p);
                CHECK(oi.texcoord_index + 1 == mi.t);
                CHECK(oi.normal_index + 1 == mi.n);
                
                if (mi.p)
                {
                    CHECK(o->attrib.vertices[3 * oi.vertex_index + 0] == m->positions[3 * mi.p + 0]);
                    CHECK(o->attrib.vertices[3 * oi.vertex_index + 1] == m->positions[3 * mi.p + 1]);
                    CHECK(o->attrib.vertices[3 * oi.vertex_index + 2] == m->positions[3 * mi.p + 2]);
                }
                
                if (mi.t)
                {
                    CHECK(o->attrib.texcoords[2 * oi.texcoord_index + 0] == m->texcoords[2 * mi.t + 0]);
                    CHECK(o->attrib.texcoords[2 * oi.texcoord_index + 1] == m->texcoords[2 * mi.t + 1]);
                }
                
                if (mi.n)
                {
                    CHECK(o->attrib.normals[3 * oi.normal_index + 0] == m->normals[3 * mi.n + 0]);
                    CHECK(o->attrib.normals[3 * oi.normal_index + 1] == m->normals[3 * mi.n + 1]);
                    CHECK(o->attrib.normals[3 * oi.normal_index + 2] == m->normals[3 * mi.n + 2]);
                }
                
                idx++;
            }
        }
    }
}

#endif //MAIN_H
